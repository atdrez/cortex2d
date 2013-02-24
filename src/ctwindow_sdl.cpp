#include <stdio.h>
#include <stdlib.h>
#include "ctGL.h"
#include "ctwindow.h"
#include "ctwindow_sdl_p.h"
#include "ctapplication.h"
#include "ctapplication_p.h"
#include "ctopenglfunctions.h"


CtWindowSdlPrivate::CtWindowSdlPrivate(CtWindow *q)
    : CtWindowPrivate(q),
      window(0),
      renderer(0),
      context(0)
{

}

bool CtWindowSdlPrivate::init(const char *title, int width, int height)
{
#ifdef CT_ANDROID
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
#elif CT_IPHONE
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS;
#else
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
#endif

    window = static_cast<SDL_Window *>(SDL_CreateWindow(title, 0, 0, width, height, flags));
    CT_ASSERT(!window, "Unable to create window");

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
    renderer = SDL_CreateRenderer(window, -1, 0);

    context = (SDL_GLContext *)SDL_GL_CreateContext(window);
    CT_ASSERT(!context, "Unable to create window");

    updateWindowSize();

    CtApplication *app = CtApplication::instance();

    if (app) {
        CtApplicationPrivate::dptr(app)->addWindow(this);
    } else {
        CT_FATAL("A CtApplication is needed before creating a window");
    }

    CtGL::assignFunctions(SDL_GL_GetProcAddress);

    return true;
}

void CtWindowSdlPrivate::release()
{
    CtApplication *app = CtApplication::instance();

    if (app)
        CtApplicationPrivate::dptr(app)->removeWindow(this);

    if (window) {
        SDL_DestroyWindow(window);
        window = 0;
    }

    if (context) {
        SDL_GL_DeleteContext(context);
        context = 0;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = 0;
    }
}

bool CtWindowSdlPrivate::makeCurrent()
{
    if (!window)
        return false;

    bool result = (SDL_GL_MakeCurrent(window, context) == 0);

    if (!result)
        CT_FATAL("CtWindow::makeCurrent(): %s" << CT_ENDL << SDL_GetError());

    return result;
}

bool CtWindowSdlPrivate::prepareGL()
{
    if (!makeCurrent())
        return false;

    CtGL::glViewport(0, 0, width, height);

    CtGL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CtGL::glEnable(GL_BLEND);
    CtGL::glEnable(0x8642); // Enable pointSize

    CtGL::glClearColor(1.0, 1.0, 1.0, 1.0);
    CtGL::glClear(GL_COLOR_BUFFER_BIT);

    return true;
}

void CtWindowSdlPrivate::presentGL()
{
    if (!window)
        return;

    SDL_GL_SwapWindow(window);
}

void CtWindowSdlPrivate::updateWindowSize()
{
    if (!window)
        return;

    SDL_GetWindowSize(window, &width, &height);
    render();
}
