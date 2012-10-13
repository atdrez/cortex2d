#ifndef FLSOUNDMIXER_SDL_P_H
#define FLSOUNDMIXER_SDL_P_H

#include "fllist.h"
#include "flsoundmixer_p.h"
#include "SDL_audio.h"

struct FlSoundMixerSdlPrivate : public FlSoundMixerPrivate
{
    FlSoundMixerSdlPrivate();
    ~FlSoundMixerSdlPrivate();

    bool open();
    void close();

    void setSpec(int frequency, FlSoundMixer::Format format,
                 int channels, int samples);

    bool isActive() const;
    void setActive(bool active);

    void addSample(FlSoundSample *sample);
    void removeSample(FlSoundSample *sample);

    bool isOpen;
    bool active;
    SDL_AudioSpec audioSpec;
    FlList<FlSoundSample *> sampleList;
};

#endif
