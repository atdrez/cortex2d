#ifndef CTWINDOW_P_H
#define CTWINDOW_P_H

#include "ctglobal.h"

class CtWindow;

struct CtWindowPrivate
{
    CtWindowPrivate(CtWindow *q);
    virtual ~CtWindowPrivate();

    virtual bool init(const char *title, int width, int height) = 0;
    virtual void release() = 0;

    void render();
    void advance(ctuint ms);

    virtual bool prepareGL() = 0;
    virtual void presentGL() = 0;
    virtual bool makeCurrent() = 0;

    virtual void setContentScaleFactor(float value) = 0;

    static CtWindowPrivate *dptr(CtWindow *window);

    CtWindow *q_ptr;
    int width;
    int height;
    void *userData;
    bool minimized;

    CT_PRIVATE_COPY(CtWindowPrivate);
};

#endif
