#ifndef FLAPPLICATION_SDL_P_H
#define FLAPPLICATION_SDL_P_H

#include "SDL.h"
#include "flglobal.h"
#include "flmap.h"
#include "flwindow_sdl_p.h"
#include "flapplication_p.h"


struct FlApplicationSdlPrivate : public FlApplicationPrivate
{
    FlApplicationSdlPrivate(FlApplication *q);

    bool init(int argc, char **argv);
    int exec();
    void quit();
    void requestQuit();

    void processEvent(const SDL_Event &event);
    FlWindowSdlPrivate *findWindowById(int winId);

    bool m_quit;
    int m_touchId;
    FlMap<int, FlPointReal> m_touchPoints;
    FlPointReal mousePressPos;
    Fl::MouseButton mouseButton;
};

#endif
