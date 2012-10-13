#ifndef FLSOUNDSAMPLE_SDL_P_H
#define FLSOUNDSAMPLE_SDL_P_H

#include "flsoundsample_p.h"
#include "SDL_audio.h"

struct FlSoundSampleSdlPrivate : public FlSoundSamplePrivate
{
    FlSoundSampleSdlPrivate();
    ~FlSoundSampleSdlPrivate();

    void play();
    void stop();
    void pause();

    bool isRunning() const;

    int loopCount() const;
    void setLoopCount(int count);

    bool loadWav(const FlString &path);

    FlString filePath;

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
