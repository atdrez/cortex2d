#ifndef FLSOUNDSAMPLE_P_H
#define FLSOUNDSAMPLE_P_H

#include "flsoundsample.h"

struct FlSoundSamplePrivate
{
    FlSoundSamplePrivate() {}
    virtual ~FlSoundSamplePrivate() {}

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;

    virtual bool isRunning() const = 0;

    virtual int loopCount() const = 0;
    virtual void setLoopCount(int count) = 0;

    virtual bool loadWav(const FlString &path) = 0;

    inline static FlSoundSamplePrivate *dptr(FlSoundSample *q) { return q->d_ptr; }
};

#endif
