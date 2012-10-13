#include <stdio.h>
#include <stdlib.h>
#include "flitem.h"
#include "flitem_p.h"
#include "flwindow.h"
#include "flapplication.h"
#include "flapplication_p.h"
#include "flopenglfunctions.h"

#ifdef FL_SDL_BACKEND
#    include "flwindow_sdl_p.h"
#endif


/******************************************************************************
 * FlWindowPrivate
 *****************************************************************************/

FlWindowPrivate::FlWindowPrivate(FlWindow *q)
    : q_ptr(q),
      userData(0)
{

}

void FlWindowPrivate::render()
{
    updateViewPort();
    q_ptr->paint();
}

void FlWindowPrivate::advance(fluint ms)
{
    q_ptr->advance(ms);
}

/******************************************************************************
 * FlWindow
 *****************************************************************************/

FlWindow::FlWindow(const char *title, int width, int height)
#ifdef FL_SDL_BACKEND
    : d_ptr(new FlWindowSdlPrivate(this))
#else
    : d_ptr(0)
#endif
{
    FL_D(FlWindow);
    FL_ASSERT(!d_ptr, "Invalid d-pointer for FlWindow");
    d->init(title, width, height);
}

FlWindow::FlWindow(FlWindowPrivate *dd)
    : d_ptr(dd)
{

}

FlWindow::~FlWindow()
{
    FL_D(FlWindow);
    d->release();
    delete d;
}

int FlWindow::width() const
{
    FL_D(FlWindow);
    return d->width;
}

int FlWindow::height() const
{
    FL_D(FlWindow);
    return d->height;
}

void FlWindow::show()
{
    FL_D(FlWindow);
    d->makeCurrent();
    init();
}

bool FlWindow::init()
{
    return true;
}

void FlWindow::shutdown()
{

}

void FlWindow::advance(fluint ms)
{
    FL_UNUSED(ms);
}

bool FlWindow::event(FlEvent *event)
{
    FL_D(FlWindow);
    event->setAccepted(true);

    switch (event->type()) {
    case FlEvent::WindowResize:
        resizeEvent(static_cast<FlWindowResizeEvent *>(event));
        break;
    case FlEvent::WindowClose:
        closeEvent(static_cast<FlWindowCloseEvent *>(event));
        if (event->isAccepted())
            d->release();
        break;
    default:
        break;
    }

    return event->isAccepted();
}

void FlWindow::closeEvent(FlWindowCloseEvent *event)
{
    FL_UNUSED(event);
}

void FlWindow::resizeEvent(FlWindowResizeEvent *event)
{
    FL_D(FlWindow);
    FL_UNUSED(event);

    d->updateViewPort();
    d->render();
}

void FlWindow::paint()
{

}

void FlWindow::swapBuffers()
{
    FL_D(FlWindow);
    d->swapBuffers();
}
