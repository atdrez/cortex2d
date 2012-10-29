#ifndef CTSOUNDMIXER_P_H
#define CTSOUNDMIXER_P_H

#include "ctsoundmixer.h"

struct CtSoundMixerPrivate
{
    CtSoundMixerPrivate() {}
    virtual ~CtSoundMixerPrivate() {}

    virtual bool open() = 0;
    virtual void close() = 0;

    virtual void setSpec(int frequency, CtSoundMixer::Format format,
                         int channels, int samples) = 0;

    virtual void addSample(CtSoundSample *sample) = 0;
    virtual void removeSample(CtSoundSample *sample) = 0;

    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;

    inline static CtSoundMixerPrivate *dptr(CtSoundMixer *q) { return q->d_ptr; }
};

#endif
