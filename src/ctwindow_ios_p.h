#ifndef CTWINDOW_IOS_P_H
#define CTWINDOW_IOS_P_H

#include "ctwindow_p.h"

class CtWindow;
struct CtIOSWindowWrapper;

struct CtWindowIOSPrivate : public CtWindowPrivate
{
    CtWindowIOSPrivate(CtWindow *q);
    ~CtWindowIOSPrivate();

    bool init(const char *title, int width, int height);
    void release();

    virtual bool prepareGL();
    virtual void presentGL();
    virtual bool makeCurrent();

    void updateWindowSize();

    void setContentScaleFactor(float value);

    struct CtIOSWindowWrapper *dw;
};

#endif
