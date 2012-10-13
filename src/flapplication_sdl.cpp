#include <string.h>
#include "flwindow.h"
#include "flwindow_sdl_p.h"
#include "flapplication_sdl_p.h"


FlApplicationSdlPrivate::FlApplicationSdlPrivate(FlApplication *q)
    : FlApplicationPrivate(q),
      m_quit(false),
      m_touchId(-1),
      mouseButton(Fl::NoButton)
{

}

bool FlApplicationSdlPrivate::init(int argc, char **argv)
{
    FlApplicationPrivate::init(argc, argv);
    FL_ASSERT(SDL_Init(SDL_INIT_AUDIO) < 0, "Unable to initialize SDL audio");
    FL_ASSERT(SDL_Init(SDL_INIT_VIDEO) < 0, "Unable to initialize SDL video");
    return true;
}

void FlApplicationSdlPrivate::quit()
{
    FlApplicationPrivate::quit();
    SDL_AudioQuit();
    SDL_VideoQuit();
}

void FlApplicationSdlPrivate::requestQuit()
{
    m_quit = true;
}

int FlApplicationSdlPrivate::exec()
{
    SDL_Event event;
    bool done = false;
    Uint32 previousTick = SDL_GetTicks();

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

        foreach (FlWindowPrivate *ww, windows) {
            FlWindowSdlPrivate *window = static_cast<FlWindowSdlPrivate *>(ww);

            if (window->makeCurrent()) {
                window->advance(deltaMs);
                window->render();
            }
        }

        previousTick = currentTick;

        // delay if renderTime < 16ms
        Uint32 renderTime = SDL_GetTicks() - previousTick;

        if (renderTime < 16)
            SDL_Delay(16 - renderTime);
    }

    quit();
    return 1;
}

FlWindowSdlPrivate *FlApplicationSdlPrivate::findWindowById(int winId)
{
#ifdef FL_ANDROID
    // XXX: SDL has a bug and it's delivering events with winID = 0
    // This should be fixed in order to handle more than a window correctly
    if (winId == 0) {
        if (windows.empty())
            return 0;
        else
            return  static_cast<FlWindowSdlPrivate *>(windows.front());
    }
#endif

    foreach (FlWindowPrivate *ww, windows) {
        FlWindowSdlPrivate *window = static_cast<FlWindowSdlPrivate *>(ww);

        if (winId == (int)SDL_GetWindowID(window->window))
            return window;
    }
    return 0;
}

void FlApplicationSdlPrivate::processEvent(const SDL_Event &event)
{
    switch (event.type) {
    case SDL_MOUSEMOTION: {
        FlWindowSdlPrivate *w = findWindowById(event.button.windowID);

        if (w) {
            if (event.motion.state != SDL_PRESSED) {
                // hover event XXX
            } else {
                FlMouseEvent ev(FlEvent::MouseMove, mouseButton,
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
        FlWindowSdlPrivate *w = findWindowById(event.button.windowID);

        if (w) {
            Fl::MouseButton button = Fl::NoButton;

            if (event.button.button == 1)
                button = Fl::ButtonLeft;
            else if (event.button.button == 2)
                button = Fl::ButtonMiddle;
            else if (event.button.button == 3)
                button = Fl::ButtonRight;

            FlPointReal pos(event.button.x, event.button.y);

            if (!isUp) {
                mouseButton = button;
                mousePressPos = pos;
            }

            FlMouseEvent ev(!isUp ? FlEvent::MousePress : FlEvent::MouseRelease,
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
            FL_WARNING("Received touch event while processing another")
            break;
        }

        SDL_Touch *touch = SDL_GetTouch(touchId);
        FlWindowSdlPrivate *w = findWindowById(event.tfinger.windowID);

        if (w && touch) {
            bool nonePressed = true;
            FlTouchPointList points;

            // retrieve touch points
            for (int i = 0; i < touch->num_fingers; i++) {
                SDL_Finger *finger = touch->fingers[i];

                if (!finger)
                    continue;

                const flreal fx = (w->width * finger->x) / flreal(touch->xres);
                const flreal fy = (w->height * finger->y) / flreal(touch->yres);
                FlPointReal firstPress = m_touchPoints.value(finger->id, FlPointReal());

                if (!firstPress.isValid()) {
                    firstPress = FlPointReal(fx, fy);
                    m_touchPoints[finger->id] = firstPress;
                }

                FlTouchPoint point(finger->id, fx, fy, firstPress.x(), firstPress.y(),
                                   finger->pressure, finger->down);
                points.push_back(point);

                if (finger->down)
                    nonePressed = false;
            }

            FlEvent::Type type;
            if (m_touchId < 0) {
                m_touchId = touchId;
                type = FlEvent::TouchBegin;
            } else if (nonePressed) {
                m_touchId = -1;
                m_touchPoints.clear();
                type = FlEvent::TouchEnd;
            } else {
                m_touchId = touchId;
                type = FlEvent::TouchUpdate;
            }

            FlTouchEvent ev(type, touchId, points);
            postEvent(w, &ev);
        }
        break;
    }
    case SDL_KEYUP:
    case SDL_KEYDOWN: {
        bool keyUp = (event.type == SDL_KEYUP);
        FlWindowSdlPrivate *w = findWindowById(event.key.windowID);

        int modifiers = Fl::NoModifier;

        if ((event.key.keysym.mod & KMOD_SHIFT))
            modifiers |= Fl::ShiftModifier;

        if ((event.key.keysym.mod & KMOD_CTRL))
            modifiers |= Fl::ControlModifier;

        if ((event.key.keysym.mod & KMOD_ALT))
            modifiers |= Fl::AltModifier;

        if (w) {
            FlKeyEvent ev(!keyUp ? FlEvent::KeyPress : FlEvent::KeyRelease,
                          (int)event.key.keysym.sym, modifiers, event.key.repeat);
            postEvent(w, &ev);
        }
        break;
    }
    case SDL_WINDOWEVENT:
        switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED: {
            FlWindowSdlPrivate *w = findWindowById(event.window.windowID);
            if (w) {
                if (!w->makeCurrent())
                    break;
                FlWindowResizeEvent ev(event.window.data1, event.window.data2);
                postEvent(w, &ev);
                break;
            }
            break;
        }
        case SDL_WINDOWEVENT_CLOSE: {
            FlWindowSdlPrivate *w = findWindowById(event.window.windowID);
            if (w) {
                FlWindowCloseEvent ev;
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
