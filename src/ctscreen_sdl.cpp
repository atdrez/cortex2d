#include "ctscreen.h"
#include "SDL.h"

struct CtScreenPrivate
{

};


CtScreen::CtScreen()
    : d(new CtScreenPrivate)
{

}

CtScreen::~CtScreen()
{
    delete d;
}

int CtScreen::width()
{
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);

    return mode.w;
}

int CtScreen::height()
{
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);

    return mode.h;
}
