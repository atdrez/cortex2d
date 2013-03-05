#include "ctwindow_ios_p.h"
#include "ctopenglfunctions.h"
#include "ctapplication.h"
#include "ctapplication_p.h"
#include "ctioswindow.h"
#include "ctiosglview.h"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

struct CtIOSWindowWrapper
{
    CtIOSWindow *window;
};

CtWindowIOSPrivate::CtWindowIOSPrivate(CtWindow *q)
   : CtWindowPrivate(q)
{
    dw = new CtIOSWindowWrapper;
    dw->window = 0;
}

CtWindowIOSPrivate::~CtWindowIOSPrivate()
{
    if (dw->window) {
        [dw->window release];
        dw->window = 0;
    }
}

bool CtWindowIOSPrivate::init(const char *title, int width, int height)
{
    dw->window = [[CtIOSWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    dw->window->glView->dd = this;

    updateWindowSize();

    CtApplication *app = CtApplication::instance();

    if (app) {
        CtApplicationPrivate::dptr(app)->addWindow(this);
    } else {
        CT_FATAL("A CtApplication is needed before creating a window");
    }

    return true;
}

void CtWindowIOSPrivate::release()
{
    CtApplication *app = CtApplication::instance();

    if (app)
        CtApplicationPrivate::dptr(app)->removeWindow(this);
}

bool CtWindowIOSPrivate::makeCurrent()
{
    CtIOSWindow *w = dw->window;

    if (!w)
        return false;

    [w makeCurrent];
    return true;
}

bool CtWindowIOSPrivate::prepareGL()
{
    CtIOSWindow *w = dw->window;

    if (!w)
        return false;

    [w makeCurrent];
    [w prepareGL];

    return true;
}

void CtWindowIOSPrivate::presentGL()
{
    CtIOSWindow *w = dw->window;

    if (!w)
        return;

    [w presentGL];
}

void CtWindowIOSPrivate::updateWindowSize()
{
    CtIOSWindow *w = dw->window;

    if (!w)
        return;

    CtIOSGLView *view = w->glView;

    width = [w bounds].size.width * view.contentScaleFactor;
    height = [w bounds].size.height * view.contentScaleFactor;
}

UIViewController *ctUIKitGetViewController(CtWindow *window)
{
    CtWindowIOSPrivate *d = static_cast<CtWindowIOSPrivate *>(CtWindowPrivate::dptr(window));
    return d->dw->window->viewController;
}

void CtWindowIOSPrivate::setContentScaleFactor(float value)
{
    CtIOSWindow *w = dw->window;

    if (!w)
        return;

    CtIOSGLView *view = w->glView;

    if (view.contentScaleFactor == value)
        return;

    view.contentScaleFactor = value;
    view.layer.contentsScale = value;
    [view setNeedsLayout];

    updateWindowSize();
}
