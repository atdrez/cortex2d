#ifndef CTSOUNDSAMPLE_P_H
#define CTSOUNDSAMPLE_P_H

#include "ctsoundsample.h"

struct CtSoundSamplePrivate
{
    CtSoundSamplePrivate() {}
    virtual ~CtSoundSamplePrivate() {}

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;

    virtual bool isRunning() const = 0;

    virtual int loopCount() const = 0;
    virtual void setLoopCount(int count) = 0;

    virtual bool loadWav(const CtString &path) = 0;

    inline static CtSoundSamplePrivate *dptr(CtSoundSample *q) { return q->d_ptr; }
};

#endif
