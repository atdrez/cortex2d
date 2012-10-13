#ifndef FLSOUNDSAMPLE_H
#define FLSOUNDSAMPLE_H

#include "flglobal.h"

class FlSoundMixer;
class FlSoundSamplePrivate;

class FlSoundSample
{
public:
    FlSoundSample();
    ~FlSoundSample();

    void play();
    void stop();
    void pause();

    bool isRunning() const;

    int loopCount() const;
    void setLoopCount(int count);

    bool loadWav(const FlString &path);

private:
    FlSoundSamplePrivate *d_ptr;
    friend class FlSoundSamplePrivate;
};

#endif
