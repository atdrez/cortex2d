#ifndef CTAPPLICATION_IOS_P_H
#define CTAPPLICATION_IOS_P_H

#include "ctglobal.h"
#include "ctmap.h"
#include "ctapplication_p.h"


struct CtApplicationIOSPrivate : public CtApplicationPrivate
{
    CtApplicationIOSPrivate(CtApplication *q);

    bool init(int argc, char **argv);
    void quit();
    void requestQuit();

    void tick(ctuint ms);

    void openURL(const char *url);

    static CtApplicationIOSPrivate *instance();

    bool m_quit;
    bool m_ready;
};

#endif
