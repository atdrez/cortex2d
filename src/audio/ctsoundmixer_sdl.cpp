#include "ctsoundmixer_sdl_p.h"
#include "ctsoundsample_sdl_p.h"

void SDLCALL ct_sound_mixer_callback(void *data, Uint8 *stream, int length)
{
    CtSoundMixerSdlPrivate *mixer = static_cast<CtSoundMixerSdlPrivate *>(data);

    memset(stream, 0, length);

    foreach (CtSoundSample *ssp, mixer->sampleList) {
        CtSoundSamplePrivate *dptr = CtSoundSamplePrivate::dptr(ssp);
        CtSoundSampleSdlPrivate *sample = static_cast<CtSoundSampleSdlPrivate *>(dptr);

        if (!sample->running | !sample->loaded || sample->bufferLength == 0)
            continue;

        int remaining = length;
        Uint8 *bufferPtr = stream;
        int bytesToConsume = sample->bufferLength - sample->soundPos;

        while (bytesToConsume <= remaining) {
            SDL_MixAudioFormat(bufferPtr, sample->buffer + sample->soundPos,
                               mixer->audioSpec.format,
                               bytesToConsume, SDL_MIX_MAXVOLUME);

            // increase pointer
            remaining -= bytesToConsume;
            bufferPtr += bytesToConsume;

            // reset sample pos
            sample->soundPos = 0;
            bytesToConsume = sample->bufferLength;

            // check if it time to finish
            if (sample->totalLoopCount >= 0) {
                sample->currentLoop++;

                if (sample->currentLoop >= sample->totalLoopCount) {
                    sample->running = false;
                    sample->currentLoop = 0;
                    break;
                }
            }
        }

        // mix remaining bytes
        if (sample->running && remaining > 0) {
            SDL_MixAudioFormat(bufferPtr, sample->buffer + sample->soundPos,
                               mixer->audioSpec.format,
                               remaining, SDL_MIX_MAXVOLUME);
            sample->soundPos += remaining;
        }
    }
}


CtSoundMixerSdlPrivate::CtSoundMixerSdlPrivate()
    : CtSoundMixerPrivate(),
      isOpen(false),
      active(false)
{
    audioSpec.freq = 16000;
    audioSpec.format = AUDIO_S16LSB;
    audioSpec.channels = 1;
    audioSpec.silence = 0;
    audioSpec.samples = 4096;
    audioSpec.padding = 0;
    audioSpec.size = 0;
    audioSpec.userdata = this;
    audioSpec.callback = ct_sound_mixer_callback;
}

CtSoundMixerSdlPrivate::~CtSoundMixerSdlPrivate()
{
    close();
}

void CtSoundMixerSdlPrivate::addSample(CtSoundSample *sample)
{
    SDL_LockAudio();
    sampleList.remove(sample);
    sampleList.push_back(sample);
    SDL_UnlockAudio();
}

void CtSoundMixerSdlPrivate::removeSample(CtSoundSample *sample)
{
    SDL_LockAudio();
    sampleList.remove(sample);
    SDL_UnlockAudio();
}

bool CtSoundMixerSdlPrivate::isActive() const
{
    return active;
}

void CtSoundMixerSdlPrivate::setActive(bool enabled)
{
    if (!isOpen)
        return;

    active = enabled;

    if (active) {
        SDL_PauseAudio(0);
    } else {
        SDL_PauseAudio(1);
    }
}

bool CtSoundMixerSdlPrivate::open()
{
    if (isOpen)
        return true;

    if (SDL_OpenAudio(&audioSpec, NULL) < 0)
        return false;

    isOpen = true;
    active = true;
    SDL_PauseAudio(0);
    return true;
}

void CtSoundMixerSdlPrivate::close()
{
    if (isOpen) {
        isOpen = false;
        SDL_CloseAudio();
    }
}

void CtSoundMixerSdlPrivate::setSpec(int frequency, CtSoundMixer::Format format,
                                     int channels, int samples)
{
    if (isOpen)
        return;

    audioSpec.freq = frequency;
    audioSpec.samples = samples;
    audioSpec.channels = channels;

    switch (format) {
    case CtSoundMixer::U8:
        audioSpec.format = AUDIO_U8;
        break;
    case CtSoundMixer::S8:
        audioSpec.format = AUDIO_S8;
        break;
    case CtSoundMixer::U16LE:
        audioSpec.format = AUDIO_U16LSB;
        break;
    case CtSoundMixer::S16LE:
        audioSpec.format = AUDIO_S16LSB;
        break;
    case CtSoundMixer::S32LE:
        audioSpec.format = AUDIO_S32LSB;
        break;
    default:
        audioSpec.format = AUDIO_S16LSB;
        break;
    }
}
