#include "flsoundmixer_sdl_p.h"
#include "flsoundsample_sdl_p.h"

void SDLCALL fl_sound_mixer_callback(void *data, Uint8 *stream, int length)
{
    FlSoundMixerSdlPrivate *mixer = static_cast<FlSoundMixerSdlPrivate *>(data);

    memset(stream, 0, length);

    foreach (FlSoundSample *ssp, mixer->sampleList) {
        FlSoundSamplePrivate *dptr = FlSoundSamplePrivate::dptr(ssp);
        FlSoundSampleSdlPrivate *sample = static_cast<FlSoundSampleSdlPrivate *>(dptr);

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


FlSoundMixerSdlPrivate::FlSoundMixerSdlPrivate()
    : FlSoundMixerPrivate(),
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
    audioSpec.callback = fl_sound_mixer_callback;
}

FlSoundMixerSdlPrivate::~FlSoundMixerSdlPrivate()
{
    close();
}

void FlSoundMixerSdlPrivate::addSample(FlSoundSample *sample)
{
    SDL_LockAudio();
    sampleList.remove(sample);
    sampleList.push_back(sample);
    SDL_UnlockAudio();
}

void FlSoundMixerSdlPrivate::removeSample(FlSoundSample *sample)
{
    SDL_LockAudio();
    sampleList.remove(sample);
    SDL_UnlockAudio();
}

bool FlSoundMixerSdlPrivate::isActive() const
{
    return active;
}

void FlSoundMixerSdlPrivate::setActive(bool enabled)
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

bool FlSoundMixerSdlPrivate::open()
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

void FlSoundMixerSdlPrivate::close()
{
    if (isOpen) {
        isOpen = false;
        SDL_CloseAudio();
    }
}

void FlSoundMixerSdlPrivate::setSpec(int frequency, FlSoundMixer::Format format,
                                     int channels, int samples)
{
    if (isOpen)
        return;

    audioSpec.freq = frequency;
    audioSpec.samples = samples;
    audioSpec.channels = channels;

    switch (format) {
    case FlSoundMixer::U8:
        audioSpec.format = AUDIO_U8;
        break;
    case FlSoundMixer::S8:
        audioSpec.format = AUDIO_S8;
        break;
    case FlSoundMixer::U16LE:
        audioSpec.format = AUDIO_U16LSB;
        break;
    case FlSoundMixer::S16LE:
        audioSpec.format = AUDIO_S16LSB;
        break;
    case FlSoundMixer::S32LE:
        audioSpec.format = AUDIO_S32LSB;
        break;
    default:
        audioSpec.format = AUDIO_S16LSB;
        break;
    }
}
