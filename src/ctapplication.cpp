#include "ctapplication.h"

#ifdef CT_SDL_BACKEND
#    include "ctapplication_sdl_p.h"
#endif

static CtApplication *ct_application = 0;

/******************************************************************************
 * CtApplication
 *****************************************************************************/

CtApplication::CtApplication(int argc, char **argv)
#ifdef CT_SDL_BACKEND
    : d_ptr(new CtApplicationSdlPrivate(this))
#else
    : d_ptr(0)
#endif
{
    CT_D(CtApplication);
    CT_ASSERT(!d_ptr, "Invalid d-pointer for CtApplication");

    if (ct_application)
        CT_FATAL("CtApplication is already created");

    d->init(argc, argv);
    ct_application = this;
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

int CtApplication::exec()
{
    CT_D(CtApplication);
    return d->exec();
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
