#include <stdio.h>
#include <stdlib.h>
#include "ctitem.h"
#include "ctitem_p.h"
#include "ctwindow.h"
#include "ctapplication.h"
#include "ctapplication_p.h"
#include "ctopenglfunctions.h"

#if defined(CT_IOS_BACKEND)
#    include "ctwindow_ios_p.h"
#elif defined(CT_SDL_BACKEND)
#    include "ctwindow_sdl_p.h"
#endif


/******************************************************************************
 * CtWindowPrivate
 *****************************************************************************/

CtWindowPrivate::CtWindowPrivate(CtWindow *q)
    : q_ptr(q),
      width(0),
      height(0),
      userData(0),
      minimized(false)
{

}

CtWindowPrivate::~CtWindowPrivate()
{

}

void CtWindowPrivate::render()
{
    prepareGL();
    q_ptr->paint();
    presentGL();
}

void CtWindowPrivate::advance(ctuint ms)
{
    q_ptr->advance(ms);
}

CtWindowPrivate *CtWindowPrivate::dptr(CtWindow *window)
{
    return window->d_ptr;
}

/******************************************************************************
 * CtWindow
 *****************************************************************************/

CtWindow::CtWindow(const char *title, int width, int height)
#if defined(CT_IOS_BACKEND)
    : d_ptr(new CtWindowIOSPrivate(this))
#elif defined(CT_SDL_BACKEND)
    : d_ptr(new CtWindowSdlPrivate(this))
#else
    : d_ptr(0)
#endif
{
    CT_ASSERT(d_ptr != 0);

    CT_D(CtWindow);
    d->init(title, width, height);
}

CtWindow::CtWindow(CtWindowPrivate *dd)
    : d_ptr(dd)
{

}

CtWindow::~CtWindow()
{
    CT_D(CtWindow);
    d->release();
    delete d;
}

int CtWindow::width() const
{
    CT_D(CtWindow);
    return d->width;
}

int CtWindow::height() const
{
    CT_D(CtWindow);
    return d->height;
}

void CtWindow::show()
{
    CT_D(CtWindow);
    d->makeCurrent();
    init();
}

bool CtWindow::isMinimized() const
{
    CT_D(CtWindow);
    return d->minimized;
}

bool CtWindow::init()
{
    return true;
}

void CtWindow::shutdown()
{

}

void CtWindow::advance(ctuint ms)
{
    CT_UNUSED(ms);
}

bool CtWindow::event(CtEvent *event)
{
    CT_D(CtWindow);
    event->setAccepted(true);

    switch (event->type()) {
    case CtEvent::WindowResize:
        resizeEvent(static_cast<CtWindowResizeEvent *>(event));
        break;
    case CtEvent::WindowClose:
        closeEvent(static_cast<CtWindowCloseEvent *>(event));
        if (event->isAccepted())
            d->release();
        break;
    case CtEvent::WindowRestore:
        d->minimized = false;
        restoreEvent(static_cast<CtWindowRestoreEvent *>(event));
        break;
    case CtEvent::WindowMinimize:
        d->minimized = true;
        minimizeEvent(static_cast<CtWindowMinimizeEvent *>(event));
        break;
    default:
        break;
    }

    return event->isAccepted();
}

void CtWindow::closeEvent(CtWindowCloseEvent *event)
{
    CT_UNUSED(event);
}

void CtWindow::resizeEvent(CtWindowResizeEvent *event)
{
    CT_UNUSED(event);
}

void CtWindow::restoreEvent(CtWindowRestoreEvent *event)
{
    CT_UNUSED(event);
}

void CtWindow::minimizeEvent(CtWindowMinimizeEvent *event)
{
    CT_UNUSED(event);
}

void CtWindow::paint()
{

}

void CtWindow::setScaleFactor(float value)
{
    CT_D(CtWindow);
    d->setContentScaleFactor(value);
}
