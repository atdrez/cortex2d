#include "ctsoundsample.h"

#ifdef CT_OPENAL_AUDIO
#include "ctsoundsample_openal_p.h"
#else
#include "ctsoundsample_sdl_p.h"
#endif

CtSoundSample::CtSoundSample()
#ifdef CT_OPENAL_AUDIO
    : d_ptr(new CtSoundSampleOpenALPrivate())
#else
    : d_ptr(new CtSoundSampleSdlPrivate())
#endif
{

}

CtSoundSample::~CtSoundSample()
{
    delete d_ptr;
}

void CtSoundSample::play()
{
    CT_D(CtSoundSample);
    d->play();
}

void CtSoundSample::stop()
{
    CT_D(CtSoundSample);
    d->stop();
}

void CtSoundSample::pause()
{
    CT_D(CtSoundSample);
    d->pause();
}

bool CtSoundSample::isRunning() const
{
    CT_D(CtSoundSample);
    return d->isRunning();
}

int CtSoundSample::loopCount() const
{
    CT_D(CtSoundSample);
    return d->loopCount();
}

void CtSoundSample::setLoopCount(int count)
{
    CT_D(CtSoundSample);
    d->setLoopCount(count);
}

bool CtSoundSample::loadWav(const CtString &path)
{
    CT_D(CtSoundSample);
    return d->loadWav(path);
}
