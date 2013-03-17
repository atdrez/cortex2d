#ifndef CTSOUNDMIXER_H
#define CTSOUNDMIXER_H

#include "ctglobal.h"

class CtSoundSample;
class CtSoundMixerPrivate;

class CtSoundMixer
{
public:
    enum Format {
        U8,
        S8,
        U16LE,
        S16LE,
        S32LE
    };

    void addSample(CtSoundSample *sample);
    void removeSample(CtSoundSample *sample);

    bool isActive() const;
    void setActive(bool active);

    static CtSoundMixer *instance();

    bool open();
    void close();
    void setSpecification(int frequency, Format format,
                          int channels, int samples);

private:
    CtSoundMixer();
    ~CtSoundMixer();

    CtSoundMixerPrivate *d_ptr;
    friend class CtSoundMixerPrivate;

    CT_PRIVATE_COPY(CtSoundMixer);
};

#endif

