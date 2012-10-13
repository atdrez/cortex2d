#include <stdio.h>
#include <stdlib.h>
#include "flGL.h"
#include "flwindow.h"
#include "flwindow_sdl_p.h"
#include "flapplication.h"
#include "flapplication_p.h"
#include "flopenglfunctions.h"


FlWindowSdlPrivate::FlWindowSdlPrivate(FlWindow *q)
    : FlWindowPrivate(q),
      window(0),
      renderer(0),
      context(0)
{

}

bool FlWindowSdlPrivate::init(const char *title, int width, int height)
{
#ifdef FL_ANDROID
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
#else
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
#endif

    window = static_cast<SDL_Window *>(SDL_CreateWindow(title, 0, 0, width, height, flags));
    FL_ASSERT(!window, "Unable to create window");

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
    renderer = SDL_CreateRenderer(window, -1, 0);

    context = (SDL_GLContext *)SDL_GL_CreateContext(window);
    FL_ASSERT(!context, "Unable to create window");

    updateWindowSize();

    FlApplication *app = FlApplication::instance();

    if (app) {
        FlApplicationPrivate::dptr(app)->addWindow(this);
    } else {
        FL_FATAL("A FlApplication is needed before creating a window");
    }

    FlGL::assignFunctions(SDL_GL_GetProcAddress);

    return true;
}

void FlWindowSdlPrivate::release()
{
    FlApplication *app = FlApplication::instance();

    if (app)
        FlApplicationPrivate::dptr(app)->removeWindow(this);

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

bool FlWindowSdlPrivate::makeCurrent()
{
    bool result = (SDL_GL_MakeCurrent(window, context) == 0);

    if (!result)
        FL_FATAL("FlWindow::makeCurrent(): %s" << FL_ENDL << SDL_GetError());

    return result;
}

void FlWindowSdlPrivate::updateViewPort()
{
    FlGL::glViewport(0, 0, width, height);
}

void FlWindowSdlPrivate::swapBuffers()
{
    SDL_GL_SwapWindow(window);
}

void FlWindowSdlPrivate::updateWindowSize()
{
    SDL_GetWindowSize(window, &width, &height);
}
