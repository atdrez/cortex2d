#ifndef FLWINDOW_P_H
#define FLWINDOW_P_H

#include "flitem.h"

class FlWindow;

struct FlWindowPrivate
{
    FlWindowPrivate(FlWindow *q);

    virtual bool init(const char *title, int width, int height) = 0;
    virtual void release() = 0;

    void render();
    void advance(fluint ms);

    virtual bool makeCurrent() = 0;
    virtual void updateViewPort() = 0;
    virtual void swapBuffers() = 0;

    FlWindow *q_ptr;
    int width;
    int height;
    void *userData;
};

#endif
