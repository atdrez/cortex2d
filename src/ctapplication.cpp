#include "ctapplication.h"

#if defined(CT_IOS_BACKEND)
#    include "ctapplication_ios_p.h"
#elif defined(CT_SDL_BACKEND)
#    include "ctapplication_sdl_p.h"
#endif

static CtApplication *ct_application = 0;

/******************************************************************************
 * CtApplication
 *****************************************************************************/

CtApplication::CtApplication(int argc, char **argv)
    : CtObject()
#if defined(CT_IOS_BACKEND)
    , d_ptr(new CtApplicationIOSPrivate(this))
#elif defined(CT_SDL_BACKEND)
    , d_ptr(new CtApplicationSdlPrivate(this))
#else
    , d_ptr(0)
#endif
{
    CT_D(CtApplication);
    CT_ASSERT(!d_ptr, "Invalid d-pointer for CtApplication");

    if (ct_application)
        CT_FATAL("CtApplication is already created");

    ct_application = this;

    d->activeWindow = 0;
    d->init(argc, argv);
}

CtApplication::~CtApplication()
{
    ct_application = 0;
    delete d_ptr;
}

CtApplication *CtApplication::instance()
{
    return ct_application;
}

void CtApplication::quit()
{
    CT_D(CtApplication);
    return d->requestQuit();
}

CtString CtApplication::applicationDir() const
{
    CtString result = applicationPath();
    int idx = result.find_last_of("/");

    if (idx < 0) {
        return CtString("");
    } else if (idx == 0) {
        return CtString("/");
    } else {
        result.erase(idx, result.length());
        return result;
    }
}

CtString CtApplication::applicationPath() const
{
    CT_D(CtApplication);

    CtString result;
    if (d->argc > 0)
        result = d->argv[0];
    return result;
}

bool CtApplication::event(CtEvent *event)
{
    event->setAccepted(true);

    switch (event->type()) {
    case CtEvent::ApplicationReady:
        readyEvent(event);
        break;
    case CtEvent::ApplicationRelease:
        releaseEvent(event);
        break;
    default:
        return CtObject::event(event);
    }

    return event->isAccepted();
}

void CtApplication::readyEvent(CtEvent *event)
{

}

void CtApplication::releaseEvent(CtEvent *event)
{

}

void CtApplication::openURL(const CtString &url)
{
    CtApplication *d = instance();
    d->d_ptr->openURL(url.c_str());
}

CtWindow *CtApplication::activeWindow() const
{
    CtApplication *d = instance();
    return d->d_ptr->activeWindow;
}

void CtApplication::setActiveWindow(CtWindow *window)
{
    CtApplication *d = instance();
    d->d_ptr->activeWindow = window;
}
