#ifndef CTWINDOW_P_H
#define CTWINDOW_P_H

#include "ctitem.h"

class CtWindow;

struct CtWindowPrivate
{
    CtWindowPrivate(CtWindow *q);

    virtual bool init(const char *title, int width, int height) = 0;
    virtual void release() = 0;

    void render();
    void advance(ctuint ms);

    virtual bool makeCurrent() = 0;
    virtual void updateViewPort() = 0;
    virtual void swapBuffers() = 0;

    CtWindow *q_ptr;
    int width;
    int height;
    void *userData;
    bool minimized;
};

#endif
