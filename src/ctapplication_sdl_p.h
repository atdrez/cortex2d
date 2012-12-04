#ifndef CTAPPLICATION_SDL_P_H
#define CTAPPLICATION_SDL_P_H

#include "SDL.h"
#include "ctglobal.h"
#include "ctmap.h"
#include "ctwindow_sdl_p.h"
#include "ctapplication_p.h"


struct CtApplicationSdlPrivate : public CtApplicationPrivate
{
    CtApplicationSdlPrivate(CtApplication *q);

    bool init(int argc, char **argv);
    int exec();
    void quit();
    void requestQuit();

    void openURL(const char *url);

    void processEvent(const SDL_Event &event);
    CtWindowSdlPrivate *findWindowById(int winId);

    bool m_quit;
    bool m_ready;
    int m_touchId;
    CtMap<int, CtPointReal> m_touchPoints;
    CtPointReal mousePressPos;
    Ct::MouseButton mouseButton;
};

#endif
