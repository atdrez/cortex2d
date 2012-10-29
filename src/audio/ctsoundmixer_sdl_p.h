#ifndef CTSOUNDMIXER_SDL_P_H
#define CTSOUNDMIXER_SDL_P_H

#include "ctlist.h"
#include "ctsoundmixer_p.h"
#include "SDL_audio.h"

struct CtSoundMixerSdlPrivate : public CtSoundMixerPrivate
{
    CtSoundMixerSdlPrivate();
    ~CtSoundMixerSdlPrivate();

    bool open();
    void close();

    void setSpec(int frequency, CtSoundMixer::Format format,
                 int channels, int samples);

    bool isActive() const;
    void setActive(bool active);

    void addSample(CtSoundSample *sample);
    void removeSample(CtSoundSample *sample);

    bool isOpen;
    bool active;
    SDL_AudioSpec audioSpec;
    CtList<CtSoundSample *> sampleList;
};

#endif
