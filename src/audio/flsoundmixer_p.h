#ifndef FLSOUNDMIXER_P_H
#define FLSOUNDMIXER_P_H

#include "flsoundmixer.h"

struct FlSoundMixerPrivate
{
    FlSoundMixerPrivate() {}
    virtual ~FlSoundMixerPrivate() {}

    virtual bool open() = 0;
    virtual void close() = 0;

    virtual void setSpec(int frequency, FlSoundMixer::Format format,
                         int channels, int samples) = 0;

    virtual void addSample(FlSoundSample *sample) = 0;
    virtual void removeSample(FlSoundSample *sample) = 0;

    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;

    inline static FlSoundMixerPrivate *dptr(FlSoundMixer *q) { return q->d_ptr; }
};

#endif
