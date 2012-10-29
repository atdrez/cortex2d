#ifndef CTWINDOW_SDL_P_H
#define CTWINDOW_SDL_P_H

#include "SDL.h"
#include "ctwindow_p.h"

class CtWindow;

struct CtWindowSdlPrivate : public CtWindowPrivate
{
    CtWindowSdlPrivate(CtWindow *q);

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
