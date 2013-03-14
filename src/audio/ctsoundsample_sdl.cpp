#include "ctsoundsample_sdl_p.h"

CtSoundSampleSdlPrivate::CtSoundSampleSdlPrivate()
    : CtSoundSamplePrivate(),
      loaded(false),
      buffer(0),
      bufferLength(0),
      soundPos(0),
      running(false),
      currentLoop(0),
      totalLoopCount(1)
{

}

CtSoundSampleSdlPrivate::~CtSoundSampleSdlPrivate()
{
    if (loaded) {
        SDL_LockAudio();
        loaded = false;
        SDL_FreeWAV(buffer);
        SDL_UnlockAudio();
    }
}

bool CtSoundSampleSdlPrivate::loadWav(const CtString &path)
{
    filePath = path;

    SDL_LockAudio();

    if (loaded)
        SDL_FreeWAV(buffer);

    soundPos = 0;
    running = false;
    currentLoop = 0;

#ifndef CT_ANDROID
    loaded = SDL_LoadWAV(path.data(), &audioSpec, &buffer, &bufferLength) != 0;
#else
    SDL_RWops *rw = SDL_RWFromFile(path.data(), "r");

    if (!rw) {
        loaded = false;
    } else {
        loaded = SDL_LoadWAV_RW(rw, 0, &audioSpec, &buffer, &bufferLength) != 0;

        SDL_RWclose(rw);
    }
#endif

    SDL_UnlockAudio();

    return loaded;
}

void CtSoundSampleSdlPrivate::play()
{
    if (loaded) {
        SDL_LockAudio();
        running = true;
        SDL_UnlockAudio();
    }
}

void CtSoundSampleSdlPrivate::stop()
{
    if (loaded) {
        SDL_LockAudio();
        soundPos = 0;
        running = false;
        currentLoop = 0;
        SDL_UnlockAudio();
    }
}

void CtSoundSampleSdlPrivate::pause()
{
    if (loaded) {
        SDL_LockAudio();
        running = false;
        SDL_UnlockAudio();
    }
}

bool CtSoundSampleSdlPrivate::isRunning() const
{
    return running;
}

int CtSoundSampleSdlPrivate::loopCount() const
{
    return totalLoopCount;
}

void CtSoundSampleSdlPrivate::setLoopCount(int count)
{
    if (totalLoopCount != count) {
        SDL_LockAudio();
        totalLoopCount = count;
        SDL_UnlockAudio();
    }
}
