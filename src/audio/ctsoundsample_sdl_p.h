#ifndef CTSOUNDSAMPLE_SDL_P_H
#define CTSOUNDSAMPLE_SDL_P_H

#include "ctsoundsample_p.h"
#include "SDL_audio.h"

struct CtSoundSampleSdlPrivate : public CtSoundSamplePrivate
{
    CtSoundSampleSdlPrivate();
    ~CtSoundSampleSdlPrivate();

    void play();
    void stop();
    void pause();

    bool isRunning() const;

    int loopCount() const;
    void setLoopCount(int count);

    bool loadWav(const CtString &path);

    CtString filePath;

    bool loaded;
    Uint8 *buffer;
    Uint32 bufferLength;
    SDL_AudioSpec audioSpec;
    int soundPos;
    bool running;
    int currentLoop;
    int totalLoopCount;
};

#endif
