#include "flapplication.h"

#ifdef FL_SDL_BACKEND
#    include "flapplication_sdl_p.h"
#endif

static FlApplication *fl_application = 0;

/******************************************************************************
 * FlApplication
 *****************************************************************************/

FlApplication::FlApplication(int argc, char **argv)
#ifdef FL_SDL_BACKEND
    : d_ptr(new FlApplicationSdlPrivate(this))
#else
    : d_ptr(0)
#endif
{
    FL_D(FlApplication);
    FL_ASSERT(!d_ptr, "Invalid d-pointer for FlApplication");

    if (fl_application)
        FL_FATAL("FlApplication is already created");

    d->init(argc, argv);
    fl_application = this;
}

FlApplication::~FlApplication()
{
    fl_application = 0;
    delete d_ptr;
}

FlApplication *FlApplication::instance()
{
    return fl_application;
}

int FlApplication::exec()
{
    FL_D(FlApplication);
    return d->exec();
}

void FlApplication::quit()
{
    FL_D(FlApplication);
    return d->requestQuit();
}

FlString FlApplication::applicationDir() const
{
    FlString result = applicationPath();
    int idx = result.find_last_of("/");

    if (idx < 0) {
        return FlString("");
    } else if (idx == 0) {
        return FlString("/");
    } else {
        result.erase(idx, result.length());
        return result;
    }
}

FlString FlApplication::applicationPath() const
{
    FL_D(FlApplication);

    FlString result;
    if (d->argc > 0)
        result = d->argv[0];
    return result;
}
