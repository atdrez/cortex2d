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
    CT_ASSERT(d_ptr != 0);

    if (ct_application)
        CT_FATAL("CtApplication is already created");

    ct_application = this;

    CT_D(CtApplication);
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
    int idx = result.lastIndexOf('/');

    if (idx < 0) {
        return CtString("");
    } else if (idx == 0) {
        return CtString("/");
    } else {
        result.remove(idx, result.length());
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
    CT_D(CtApplication);

    event->setAccepted(true);

    switch (event->type()) {
    case CtEvent::ApplicationReady:
        readyEvent(event);
        break;
    case CtEvent::ApplicationAboutToQuit:
        releaseEvent(event);
        break;
    case CtEvent::ApplicationActivated:
    case CtEvent::ApplicationDeactivated:
    case CtEvent::ApplicationAboutToResign:
    case CtEvent::ApplicationAboutToActivate:
        break;
    default:
        return CtObject::event(event);
    }

    d->postEventToAllWindows(event);

    return event->isAccepted();
}

void CtApplication::readyEvent(CtEvent *event)
{
    CT_UNUSED(event);
}

void CtApplication::releaseEvent(CtEvent *event)
{
    CT_UNUSED(event);
}

void CtApplication::openURL(const CtString &url)
{
    CtApplication *d = instance();
    d->d_ptr->openURL(url.data());
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
