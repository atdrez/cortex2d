#ifndef CTSOUNDSAMPLE_H
#define CTSOUNDSAMPLE_H

#include "ctglobal.h"

class CtSoundMixer;
class CtSoundSamplePrivate;

class CtSoundSample
{
public:
    CtSoundSample();
    ~CtSoundSample();

    void play();
    void stop();
    void pause();

    bool isRunning() const;

    int loopCount() const;
    void setLoopCount(int count);

    bool loadWav(const CtString &path);

private:
    CtSoundSamplePrivate *d_ptr;
    friend class CtSoundSamplePrivate;

    CT_PRIVATE_COPY(CtSoundSample);
};

#endif
