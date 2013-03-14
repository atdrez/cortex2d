#include "ctsoundmixer_openal_p.h"
#include "ctsoundsample_openal_p.h"

CtSoundMixerOpenALPrivate::CtSoundMixerOpenALPrivate()
    : isOpen(false),
      active(true)
{

}

CtSoundMixerOpenALPrivate::~CtSoundMixerOpenALPrivate()
{
    close();
}

bool CtSoundMixerOpenALPrivate::open()
{
    if (!isOpen) {
        alutInit(0, 0);
        isOpen = (alGetError() == ALC_NO_ERROR);
    }

    return isOpen;
}

void CtSoundMixerOpenALPrivate::close()
{
    if (isOpen) {
        alutExit();
        isOpen = false;
    }
}

void CtSoundMixerOpenALPrivate::setSpec(int frequency, CtSoundMixer::Format format,
                                        int channels, int samples)
{
    CT_UNUSED(frequency);
    CT_UNUSED(format);
    CT_UNUSED(channels);
}

bool CtSoundMixerOpenALPrivate::isActive() const
{
    return active;
}

void CtSoundMixerOpenALPrivate::setActive(bool active)
{
    if (this->active == active)
        return;

    this->active = active;

    foreach (CtSoundSample *sample, sampleList) {
        CtSoundSamplePrivate *d = CtSoundSamplePrivate::dptr(sample);
        CtSoundSampleOpenALPrivate *dsample = static_cast<CtSoundSampleOpenALPrivate *>(d);
        dsample->setVolume(active ? 1 : 0);
    }
}

void CtSoundMixerOpenALPrivate::addSample(CtSoundSample *sample)
{
    sampleList.append(sample);
}

void CtSoundMixerOpenALPrivate::removeSample(CtSoundSample *sample)
{
    sampleList.removeAll(sample);
}
