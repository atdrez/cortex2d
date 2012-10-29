#include <stdio.h>
#include <stdlib.h>
#include "ctitem.h"
#include "ctitem_p.h"
#include "ctwindow.h"
#include "ctapplication.h"
#include "ctapplication_p.h"
#include "ctopenglfunctions.h"

#ifdef CT_SDL_BACKEND
#    include "ctwindow_sdl_p.h"
#endif


/******************************************************************************
 * CtWindowPrivate
 *****************************************************************************/

CtWindowPrivate::CtWindowPrivate(CtWindow *q)
    : q_ptr(q),
      userData(0)
{

}

void CtWindowPrivate::render()
{
    updateViewPort();
    q_ptr->paint();
}

void CtWindowPrivate::advance(ctuint ms)
{
    q_ptr->advance(ms);
}

/******************************************************************************
 * CtWindow
 *****************************************************************************/

CtWindow::CtWindow(const char *title, int width, int height)
#ifdef CT_SDL_BACKEND
    : d_ptr(new CtWindowSdlPrivate(this))
#else
    : d_ptr(0)
#endif
{
    CT_D(CtWindow);
    CT_ASSERT(!d_ptr, "Invalid d-pointer for CtWindow");
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
    CT_D(CtWindow);
    CT_UNUSED(event);

    d->updateViewPort();
    d->render();
}

void CtWindow::paint()
{

}

void CtWindow::swapBuffers()
{
    CT_D(CtWindow);
    d->swapBuffers();
}
