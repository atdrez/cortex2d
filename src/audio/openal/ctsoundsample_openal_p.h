#ifndef CTSOUNDSAMPLE_OPENAL_P_H
#define CTSOUNDSAMPLE_OPENAL_P_H

#include "ctsoundsample_p.h"

#ifdef CT_IPHONE
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#include "alut.h"


struct CtSoundSampleOpenALPrivate : public CtSoundSamplePrivate
{
    CtSoundSampleOpenALPrivate();
    ~CtSoundSampleOpenALPrivate();

    void play();
    void stop();
    void pause();

    bool isRunning() const;

    int loopCount() const;
    void setLoopCount(int count);

    bool loadWav(const CtString &path);
    void setVolume(ctreal value);

    CtString filePath;

    ALuint buffer;
    ALuint source;
    int totalLoopCount;
};

#endif
