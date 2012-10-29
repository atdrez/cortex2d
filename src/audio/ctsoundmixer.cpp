#include "ctsoundmixer.h"
#include "ctsoundmixer_sdl_p.h"

CtSoundMixer::CtSoundMixer()
    : d_ptr(new CtSoundMixerSdlPrivate)
{

}

CtSoundMixer::~CtSoundMixer()
{
    delete d_ptr;
}

CtSoundMixer *CtSoundMixer::instance()
{
    static CtSoundMixer *result = 0;

    if (!result)
        result = new CtSoundMixer();

    return result;
}

void CtSoundMixer::addSample(CtSoundSample *sample)
{
    CT_D(CtSoundMixer);
    d->addSample(sample);
}

void CtSoundMixer::removeSample(CtSoundSample *sample)
{
    CT_D(CtSoundMixer);
    d->removeSample(sample);
}

bool CtSoundMixer::isActive() const
{
    CT_D(CtSoundMixer);
    return d->isActive();
}

void CtSoundMixer::setActive(bool active)
{
    CT_D(CtSoundMixer);
    d->setActive(active);
}

bool CtSoundMixer::open()
{
    CT_D(CtSoundMixer);
    return d->open();
}

void CtSoundMixer::close()
{
    CT_D(CtSoundMixer);
    d->close();
}

void CtSoundMixer::setSpecification(int frequency, Format format, int channels, int samples)
{
    CT_D(CtSoundMixer);
    d->setSpec(frequency, format, channels, samples);
}
