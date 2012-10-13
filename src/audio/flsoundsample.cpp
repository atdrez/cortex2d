#include "flsoundsample.h"
#include "flsoundsample_sdl_p.h"

FlSoundSample::FlSoundSample()
    : d_ptr(new FlSoundSampleSdlPrivate())
{

}

FlSoundSample::~FlSoundSample()
{
    delete d_ptr;
}

void FlSoundSample::play()
{
    FL_D(FlSoundSample);
    d->play();
}

void FlSoundSample::stop()
{
    FL_D(FlSoundSample);
    d->stop();
}

void FlSoundSample::pause()
{
    FL_D(FlSoundSample);
    d->pause();
}

bool FlSoundSample::isRunning() const
{
    FL_D(FlSoundSample);
    return d->isRunning();
}

int FlSoundSample::loopCount() const
{
    FL_D(FlSoundSample);
    return d->loopCount();
}

void FlSoundSample::setLoopCount(int count)
{
    FL_D(FlSoundSample);
    d->setLoopCount(count);
}

bool FlSoundSample::loadWav(const FlString &path)
{
    FL_D(FlSoundSample);
    return d->loadWav(path);
}
