#ifndef FLWINDOW_SDL_P_H
#define FLWINDOW_SDL_P_H

#include "SDL.h"
#include "flwindow_p.h"

class FlWindow;

struct FlWindowSdlPrivate : public FlWindowPrivate
{
    FlWindowSdlPrivate(FlWindow *q);

    bool init(const char *title, int width, int height);
    void release();

    bool makeCurrent();
    void updateViewPort();
    void swapBuffers();

    void updateWindowSize();

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_GLContext context;
};

#endif
