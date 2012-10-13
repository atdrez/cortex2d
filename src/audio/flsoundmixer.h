#ifndef FLSOUNDMIXER_H
#define FLSOUNDMIXER_H

#include "flglobal.h"

class FlSoundSample;
class FlSoundMixerPrivate;

class FlSoundMixer
{
public:
    enum Format {
        U8,
        S8,
        U16LE,
        S16LE,
        S32LE
    };

    void addSample(FlSoundSample *sample);
    void removeSample(FlSoundSample *sample);

    bool isActive() const;
    void setActive(bool active);

    static FlSoundMixer *instance();

    bool open();
    void close();
    void setSpecification(int frequency, Format format,
                          int channels, int samples);

private:
    FlSoundMixer();
    ~FlSoundMixer();

    FlSoundMixerPrivate *d_ptr;
    friend class FlSoundMixerPrivate;
};

#endif

