#ifndef CTAPPLICATION_IOS_P_H
#define CTAPPLICATION_IOS_P_H

#include "SDL.h"
#include "ctglobal.h"
#include "ctmap.h"
#include "ctwindow_sdl_p.h"
#include "ctapplication_p.h"


struct CtApplicationIOSPrivate : public CtApplicationPrivate
{
    CtApplicationIOSPrivate(CtApplication *q);

    bool init(int argc, char **argv);
    void quit();
    void requestQuit();

    void tick(ctuint ms);

    static CtApplicationIOSPrivate *instance();

    bool m_quit;
    bool m_ready;
};

#endif
