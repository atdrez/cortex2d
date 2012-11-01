#ifndef CTWINDOW_H
#define CTWINDOW_H

#include <stdlib.h>
#include "ctlist.h"
#include "ctevents.h"
#include "ctGL.h"

class CtWindow;
class CtSceneItem;
class CtWindowPrivate;
class CtApplicationPrivate;

typedef CtList<CtWindow *> CtWindowList;

class CtWindow
{
public:
    CtWindow(const char *title, int width, int height);
    virtual ~CtWindow();

    int width() const;
    int height() const;

    void show();

protected:
    CtWindow(CtWindowPrivate *dd);

    virtual bool init();
    virtual void shutdown();
    virtual void paint();
    virtual void advance(ctuint ms);

    virtual bool event(CtEvent *event);
    virtual void closeEvent(CtWindowCloseEvent *event);
    virtual void resizeEvent(CtWindowResizeEvent *event);

    void swapBuffers();

    CtWindowPrivate *d_ptr;

private:
    friend class CtApplication;
    friend class CtApplicationPrivate;
    friend class CtWindowPrivate;

    friend CtWindowPrivate *CtWindow_dptr(CtWindow *window);
};

#endif