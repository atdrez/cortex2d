#include "flsoundmixer.h"
#include "flsoundmixer_sdl_p.h"

FlSoundMixer::FlSoundMixer()
    : d_ptr(new FlSoundMixerSdlPrivate)
{

}

FlSoundMixer::~FlSoundMixer()
{
    delete d_ptr;
}

FlSoundMixer *FlSoundMixer::instance()
{
    static FlSoundMixer *result = 0;

    if (!result)
        result = new FlSoundMixer();

    return result;
}

void FlSoundMixer::addSample(FlSoundSample *sample)
{
    FL_D(FlSoundMixer);
    d->addSample(sample);
}

void FlSoundMixer::removeSample(FlSoundSample *sample)
{
    FL_D(FlSoundMixer);
    d->removeSample(sample);
}

bool FlSoundMixer::isActive() const
{
    FL_D(FlSoundMixer);
    return d->isActive();
}

void FlSoundMixer::setActive(bool active)
{
    FL_D(FlSoundMixer);
    d->setActive(active);
}

bool FlSoundMixer::open()
{
    FL_D(FlSoundMixer);
    return d->open();
}

void FlSoundMixer::close()
{
    FL_D(FlSoundMixer);
    d->close();
}

void FlSoundMixer::setSpecification(int frequency, Format format, int channels, int samples)
{
    FL_D(FlSoundMixer);
    d->setSpec(frequency, format, channels, samples);
}
