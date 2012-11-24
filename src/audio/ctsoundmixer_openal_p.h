#ifndef CTSOUNDMIXER_OPENAL_P_H
#define CTSOUNDMIXER_OPENAL_P_H

#include "ctlist.h"
#include "ctsoundmixer_p.h"

#ifdef CT_IPHONE
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#include "alut.h"


struct CtSoundMixerOpenALPrivate : public CtSoundMixerPrivate
{
    CtSoundMixerOpenALPrivate();
    ~CtSoundMixerOpenALPrivate();

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
    CtList<CtSoundSample *> sampleList;
};

#endif
