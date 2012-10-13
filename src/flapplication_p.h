#ifndef FLAPPLICATION_P_H
#define FLAPPLICATION_P_H

#include "fllist.h"
#include "flevents.h"
#include "flwindow.h"
#include "flapplication.h"

class FlWindow;
class FlWindowPrivate;

struct FlApplicationPrivate
{
    FlApplicationPrivate(FlApplication *q);

    virtual bool init(int argc, char **argv);
    virtual int exec() = 0;
    virtual void quit();

    virtual void requestQuit() {}

    void addWindow(FlWindowPrivate *window);
    void removeWindow(FlWindowPrivate *window);
    void postEvent(FlWindowPrivate *window, FlEvent *event);

    inline static FlApplicationPrivate *dptr(FlApplication *app) { return app->d_ptr; }

    FlApplication *q_ptr;
    int argc;
    char **argv;
    FlList<FlWindowPrivate *> windows;

    friend class FlWindow;
};

#endif
