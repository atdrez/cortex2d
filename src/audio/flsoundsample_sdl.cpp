#include "flsoundsample_sdl_p.h"

FlSoundSampleSdlPrivate::FlSoundSampleSdlPrivate()
    : FlSoundSamplePrivate(),
      loaded(false),
      buffer(0),
      bufferLength(0),
      soundPos(0),
      running(false),
      currentLoop(0),
      totalLoopCount(1)
{

}

FlSoundSampleSdlPrivate::~FlSoundSampleSdlPrivate()
{
    if (loaded) {
        SDL_LockAudio();
        loaded = false;
        SDL_FreeWAV(buffer);
        SDL_UnlockAudio();
    }
}

bool FlSoundSampleSdlPrivate::loadWav(const FlString &path)
{
    filePath = path;

    SDL_LockAudio();

    if (loaded)
        SDL_FreeWAV(buffer);

    soundPos = 0;
    running = false;
    currentLoop = 0;

#ifndef FL_ANDROID
    loaded = SDL_LoadWAV(path.c_str(), &audioSpec, &buffer, &bufferLength) != 0;
#else
    SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "r");

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

void FlSoundSampleSdlPrivate::play()
{
    if (loaded) {
        SDL_LockAudio();
        running = true;
        SDL_UnlockAudio();
    }
}

void FlSoundSampleSdlPrivate::stop()
{
    if (loaded) {
        SDL_LockAudio();
        soundPos = 0;
        running = false;
        currentLoop = 0;
        SDL_UnlockAudio();
    }
}

void FlSoundSampleSdlPrivate::pause()
{
    if (loaded) {
        SDL_LockAudio();
        running = false;
        SDL_UnlockAudio();
    }
}

bool FlSoundSampleSdlPrivate::isRunning() const
{
    return running;
}

int FlSoundSampleSdlPrivate::loopCount() const
{
    return totalLoopCount;
}

void FlSoundSampleSdlPrivate::setLoopCount(int count)
{
    if (totalLoopCount != count) {
        SDL_LockAudio();
        totalLoopCount = count;
        SDL_UnlockAudio();
    }
}
