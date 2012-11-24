#include "ctsoundsample_openal_p.h"
#include "ctmap.h"

typedef std::pair<ALuint, int> ALBufferEntry;
static CtMap<CtString, ALBufferEntry> ct_openalBuffers;

static ALuint ct_createALBuffer(const CtString &path)
{
    if (path.empty())
        return 0;

    ALBufferEntry entry = ct_openalBuffers.value(path, ALBufferEntry(0, 0));

    if (entry.second > 0) {
        entry.second++;
        ct_openalBuffers[path] = entry;
        return entry.first;
    }

    ALuint buffer = alutCreateBufferFromFile(path.c_str());

    if (!buffer) {
        CT_WARNING("Unable to load sound file: " << path);
        return 0;
    }

    ct_openalBuffers[path] = ALBufferEntry(buffer, 1);
    return buffer;
}

static void ct_releaseALBuffer(const CtString &path)
{
    if (path.empty())
        return;

    ALBufferEntry entry = ct_openalBuffers.value(path, ALBufferEntry(0, 0));

    if (entry.second <= 0)
        return;

    if (entry.second > 1) {
        entry.second--;
        ct_openalBuffers[path] = entry;
    } else {
        ALuint buffer = entry.first;
        alDeleteBuffers(1, &buffer);
        ct_openalBuffers.erase(path);
    }
}


CtSoundSampleOpenALPrivate::CtSoundSampleOpenALPrivate()
    : CtSoundSamplePrivate(),
      buffer(0),
      totalLoopCount(1)
{
    alGenSources(1, &source);
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
}

CtSoundSampleOpenALPrivate::~CtSoundSampleOpenALPrivate()
{
    alDeleteSources(1, &source);
    ct_releaseALBuffer(filePath);
}

bool CtSoundSampleOpenALPrivate::loadWav(const CtString &path)
{
    if (path.empty())
        return false;

    if (filePath == path)
        return bool(buffer);

    ct_releaseALBuffer(filePath);

    filePath = path;
    buffer = ct_createALBuffer(path);
    alSourcei(source, AL_BUFFER, buffer);

    return bool(buffer);
}

void CtSoundSampleOpenALPrivate::play()
{
    if (buffer)
        alSourcePlay(source);
}

void CtSoundSampleOpenALPrivate::stop()
{
    if (buffer)
        alSourceStop(source);
}

void CtSoundSampleOpenALPrivate::pause()
{
    if (buffer)
        alSourcePause(source);
}

bool CtSoundSampleOpenALPrivate::isRunning() const
{
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

int CtSoundSampleOpenALPrivate::loopCount() const
{
    return totalLoopCount;
}

void CtSoundSampleOpenALPrivate::setLoopCount(int count)
{
    if (totalLoopCount != count) {
        totalLoopCount = count;

        // XXX
        if (count < 0)
            alSourcei(source, AL_LOOPING, AL_TRUE);
        else
            alSourcei(source, AL_LOOPING, AL_FALSE);
    }
}

void CtSoundSampleOpenALPrivate::setVolume(ctreal value)
{
    alSourcef(source, AL_PITCH, value);
}
