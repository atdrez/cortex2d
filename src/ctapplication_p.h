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
    virtual void quit();

    virtual void requestQuit() {}

    void addWindow(CtWindowPrivate *window);
    void removeWindow(CtWindowPrivate *window);
    static void postEvent(CtWindowPrivate *window, CtEvent *event);

    void postEventToAllWindows(CtEvent *event);

    virtual void openURL(const char *url) = 0;

    inline static CtApplicationPrivate *dptr(CtApplication *app) { return app->d_ptr; }

    CtApplication *q_ptr;
    int argc;
    char **argv;
    CtWindow *activeWindow;
    CtList<CtWindowPrivate *> windows;

    friend class CtWindow;
};

#endif
