#include <string.h>
#include "ctwindow.h"
#include "ctwindow_sdl_p.h"
#include "ctapplication_sdl_p.h"

int ctMain(int argc, char **argv, CtApplication *app)
{
    CT_UNUSED(argc);
    CT_UNUSED(argv);

    if (!app)
        return 0;
    else
        return static_cast<CtApplicationSdlPrivate *>(app->d_ptr)->exec();
}

CtApplicationSdlPrivate::CtApplicationSdlPrivate(CtApplication *q)
    : CtApplicationPrivate(q),
      m_quit(false),
      m_ready(false),
      m_touchId(-1),
      mouseButton(Ct::NoButton)
{

}

bool CtApplicationSdlPrivate::init(int argc, char **argv)
{
    CtApplicationPrivate::init(argc, argv);
    CT_ASSERT(SDL_Init(SDL_INIT_AUDIO) < 0, "Unable to initialize SDL audio");
    CT_ASSERT(SDL_Init(SDL_INIT_VIDEO) < 0, "Unable to initialize SDL video");

    return true;
}

void CtApplicationSdlPrivate::quit()
{
    CtApplicationPrivate::quit();
    SDL_AudioQuit();
    SDL_VideoQuit();
}

void CtApplicationSdlPrivate::requestQuit()
{
    m_quit = true;
}

int CtApplicationSdlPrivate::exec()
{
    SDL_Event event;
    bool done = false;
    Uint32 previousTick = SDL_GetTicks();

    if (!m_ready) {
        CtEvent ev(CtEvent::ApplicationReady);
        CtObject::sendEvent(q_ptr, &ev);
        m_ready = true;
    }

    while (!done) {
        while (SDL_PollEvent(&event))
            processEvent(event);

        if (m_quit)
            break;

        if (windows.empty())
            break;

        Uint32 currentTick = SDL_GetTicks();
        Uint32 deltaMs = (currentTick >= previousTick) ? (currentTick - previousTick)
            : currentTick; // return to 0 after 47.9 days

        foreach (CtWindowPrivate *ww, windows) {
            CtWindowSdlPrivate *window = static_cast<CtWindowSdlPrivate *>(ww);
            window->advance(deltaMs);
            window->render();
        }

        previousTick = currentTick;

        // delay if renderTime < 16ms
        Uint32 renderTime = SDL_GetTicks() - previousTick;

        if (renderTime < 16)
            SDL_Delay(16 - renderTime);
    }

    CtEvent ev(CtEvent::ApplicationRelease);
    CtObject::sendEvent(q_ptr, &ev);

    quit();
    return 1;
}

CtWindowSdlPrivate *CtApplicationSdlPrivate::findWindowById(int winId)
{
#ifdef CT_ANDROID
    // XXX: SDL has a bug and it's delivering events with winID = 0
    // This should be fixed in order to handle more than a window correctly
    if (winId == 0) {
        if (windows.empty())
            return 0;
        else
            return  static_cast<CtWindowSdlPrivate *>(windows.front());
    }
#endif

    foreach (CtWindowPrivate *ww, windows) {
        CtWindowSdlPrivate *window = static_cast<CtWindowSdlPrivate *>(ww);

        if (winId == (int)SDL_GetWindowID(window->window))
            return window;
    }
    return 0;
}

void CtApplicationSdlPrivate::processEvent(const SDL_Event &event)
{
    switch (event.type) {
    case SDL_QUIT: {
        //quit();
        break;
    }
    case SDL_MOUSEMOTION: {
        CtWindowSdlPrivate *w = findWindowById(event.button.windowID);

        if (w) {
            if (event.motion.state != SDL_PRESSED) {
                // hover event XXX
            } else {
                CtMouseEvent ev(CtEvent::MouseMove, mouseButton,
                                event.motion.x, event.motion.y,
                                event.motion.x, event.motion.y,
                                mousePressPos.x(), mousePressPos.y());
                postEvent(w, &ev);
            }
        }
        break;
    }
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN: {
        bool isUp = (event.type == SDL_MOUSEBUTTONUP);
        CtWindowSdlPrivate *w = findWindowById(event.button.windowID);

        if (w) {
            Ct::MouseButton button = Ct::NoButton;

            if (event.button.button == 1)
                button = Ct::ButtonLeft;
            else if (event.button.button == 2)
                button = Ct::ButtonMiddle;
            else if (event.button.button == 3)
                button = Ct::ButtonRight;

            ctreal fx = event.button.x;
            ctreal fy = event.button.y;

            // XXX: ugly hack to workaround a SDL bug
#ifdef CT_RETINA_DISPLAY
            fx *= 2;
            fy *= 2;
#endif

            CtPointReal pos(fx, fy);

            if (!isUp) {
                mouseButton = button;
                mousePressPos = pos;
            }

            CtMouseEvent ev(!isUp ? CtEvent::MousePress : CtEvent::MouseRelease,
                            button, pos.x(), pos.y(), pos.x(), pos.y(),
                            mousePressPos.x(), mousePressPos.y());
            postEvent(w, &ev);
        }
        break;
    }
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
    case SDL_FINGERUP: {
        int touchId = event.tfinger.touchId;

        // XXX: assuming just one touch per time
        if (m_touchId >= 0 && m_touchId != touchId) {
            CT_WARNING("Received touch event while processing another")
            break;
        }

        SDL_Touch *touch = SDL_GetTouch(touchId);
        CtWindowSdlPrivate *w = findWindowById(event.tfinger.windowID);

        if (w && touch) {
            bool nonePressed = true;
            CtTouchPointList points;

            // retrieve touch points
            for (int i = 0; i < touch->num_fingers; i++) {
                SDL_Finger *finger = touch->fingers[i];

                if (!finger)
                    continue;

                ctreal fx = (w->width * finger->x) / ctreal(touch->xres);
                ctreal fy = (w->height * finger->y) / ctreal(touch->yres);
                CtPointReal firstPress = m_touchPoints.value(finger->id, CtPointReal());

                // XXX: ugly hack to workaround a SDL bug
#ifdef CT_RETINA_DISPLAY
                fx *= 2;
                fy *= 2;
#endif

                if (!firstPress.isValid()) {
                    firstPress = CtPointReal(fx, fy);
                    m_touchPoints[finger->id] = firstPress;
                }

                CtTouchPoint point(finger->id, fx, fy, firstPress.x(), firstPress.y(),
                                   finger->pressure, finger->down);
                points.push_back(point);

                if (finger->down)
                    nonePressed = false;
            }

            CtEvent::Type type;
            if (m_touchId < 0) {
                m_touchId = touchId;
                type = CtEvent::TouchBegin;
            } else if (nonePressed) {
                m_touchId = -1;
                m_touchPoints.clear();
                type = CtEvent::TouchEnd;
            } else {
                m_touchId = touchId;
                type = CtEvent::TouchUpdate;
            }

            CtTouchEvent ev(type, touchId, points);
            postEvent(w, &ev);
        }
        break;
    }
    case SDL_KEYUP:
    case SDL_KEYDOWN: {
        bool keyUp = (event.type == SDL_KEYUP);
        CtWindowSdlPrivate *w = findWindowById(event.key.windowID);

        int modifiers = Ct::NoModifier;

        if ((event.key.keysym.mod & KMOD_SHIFT))
            modifiers |= Ct::ShiftModifier;

        if ((event.key.keysym.mod & KMOD_CTRL))
            modifiers |= Ct::ControlModifier;

        if ((event.key.keysym.mod & KMOD_ALT))
            modifiers |= Ct::AltModifier;

        if (w) {
            CtKeyEvent ev(!keyUp ? CtEvent::KeyPress : CtEvent::KeyRelease,
                          (int)event.key.keysym.sym, modifiers, event.key.repeat);
            postEvent(w, &ev);
        }
        break;
    }
    case SDL_WINDOWEVENT:
        switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED: {
            CtWindowSdlPrivate *w = findWindowById(event.window.windowID);
            if (w) {
                w->updateWindowSize();
                CtWindowResizeEvent ev(event.window.data1, event.window.data2);
                postEvent(w, &ev);
                break;
            }
            break;
        }
        case SDL_WINDOWEVENT_CLOSE: {
            CtWindowSdlPrivate *w = findWindowById(event.window.windowID);
            if (w) {
                CtWindowCloseEvent ev;
                postEvent(w, &ev);
            }
            break;
        }
        case SDL_WINDOWEVENT_MINIMIZED: {
            CtWindowSdlPrivate *w = findWindowById(event.window.windowID);
            if (w) {
                CtWindowMinimizeEvent ev;
                postEvent(w, &ev);
            }
            break;
        }
        case SDL_WINDOWEVENT_RESTORED: {
            CtWindowSdlPrivate *w = findWindowById(event.window.windowID);
            if (w) {
                CtWindowRestoreEvent ev;
                postEvent(w, &ev);
            }
            break;
        }
        }
        break;
    default:
        break;
    }
}

void CtApplicationSdlPrivate::openURL(const char *url)
{
    CT_WARNING("TODO: Application Open URL");
}
