#ifndef CTAPPLICATION_P_H
#define CTAPPLICATION_P_H

#include "ctlist.h"
#include "ctevents.h"
#include "ctwindow.h"
#include "ctapplication.h"

class CtWindow;
class CtWindowPrivate;

struct CtApplicationPrivate
{
    CtApplicationPrivate(CtApplication *q);

    virtual bool init(int argc, char **argv);
    virtual int exec() = 0;
    virtual void quit();

    virtual void requestQuit() {}

    void addWindow(CtWindowPrivate *window);
    void removeWindow(CtWindowPrivate *window);
    void postEvent(CtWindowPrivate *window, CtEvent *event);

    inline static CtApplicationPrivate *dptr(CtApplication *app) { return app->d_ptr; }

    CtApplication *q_ptr;
    int argc;
    char **argv;
    CtList<CtWindowPrivate *> windows;

    friend class CtWindow;
};

#endif
