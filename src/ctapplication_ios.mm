#include <string.h>
#include "ctwindow.h"
#include "ctwindow_ios_p.h"
#include "ctapplication_ios_p.h"
#import <UIKit/UIKit.h>
#import "ctiosappdelegate.h"

int ctMain(int argc, char **argv, CtApplication *app)
{
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([MainAppDelegate class]));
    }
}

static CtApplicationIOSPrivate *ct_app_ios_private = 0;

CtApplicationIOSPrivate::CtApplicationIOSPrivate(CtApplication *q)
    : CtApplicationPrivate(q),
      m_quit(false),
      m_ready(false)
{
    ct_app_ios_private = this;
}

bool CtApplicationIOSPrivate::init(int argc, char **argv)
{
    return true;
}

void CtApplicationIOSPrivate::quit()
{

}

void CtApplicationIOSPrivate::requestQuit()
{
    //m_quit = true;
}

void CtApplicationIOSPrivate::tick(ctuint ms)
{
    foreach (CtWindowPrivate *w, windows) {
        w->advance(ms);
        w->render();
    }
}

CtApplicationIOSPrivate *CtApplicationIOSPrivate::instance()
{
    return ct_app_ios_private;
}
