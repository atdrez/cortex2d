#ifndef FLWINDOW_H
#define FLWINDOW_H

#include <stdlib.h>
#include "fllist.h"
#include "flevents.h"
#include "flGL.h"

class FlWindow;
class FlSceneItem;
class FlWindowPrivate;
class FlApplicationPrivate;

typedef FlList<FlWindow *> FlWindowList;

class FlWindow
{
public:
    FlWindow(const char *title, int width, int height);
    virtual ~FlWindow();

    int width() const;
    int height() const;

    void show();

protected:
    FlWindow(FlWindowPrivate *dd);

    virtual bool init();
    virtual void shutdown();
    virtual void paint();
    virtual void advance(fluint ms);

    virtual bool event(FlEvent *event);
    virtual void closeEvent(FlWindowCloseEvent *event);
    virtual void resizeEvent(FlWindowResizeEvent *event);

    void swapBuffers();

    FlWindowPrivate *d_ptr;

private:
    friend class FlApplication;
    friend class FlApplicationPrivate;
    friend class FlWindowPrivate;

    friend FlWindowPrivate *FlWindow_dptr(FlWindow *window);
};

#endif
