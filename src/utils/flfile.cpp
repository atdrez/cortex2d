#include "flfile.h"
#include "flbuffer.h"

#if defined(FL_ANDROID) || defined(FL_IPHONE)
#    include "SDL.h"
#    define FL_FILE_RW
#endif


FlFile::FlFile(const FlString &path)
    : m_path(path)
{
    m_in.fp = 0;
    m_in.rw = 0;
}

FlFile::~FlFile()
{
    close();
}

bool FlFile::open(OpenMode mode)
{
    close();

#ifndef FL_FILE_RW
    switch (mode) {
    case ReadOnly:
        m_in.fp = fopen(m_path.c_str(), "r");
        break;
    case WriteOnly:
        m_in.fp = fopen(m_path.c_str(), "w");
        break;
    default:
        m_in.fp = fopen(m_path.c_str(), "rw");
        break;
    }
    return m_in.fp;
#else
    switch (mode) {
    case ReadOnly:
        m_in.rw = SDL_RWFromFile(m_path.c_str(), "r");
        break;
    case WriteOnly:
        m_in.rw = SDL_RWFromFile(m_path.c_str(), "w");
        break;
    default:
        m_in.rw = SDL_RWFromFile(m_path.c_str(), "rw");
        break;
    }
    return m_in.rw;
#endif
}

void FlFile::close()
{
#ifndef FL_FILE_RW
    if (m_in.fp) {
        fclose(m_in.fp);
        m_in.fp = 0;
    }
#else
    if (m_in.rw) {
        SDL_RWclose(m_in.rw);
        m_in.rw = 0;
    }
#endif
}

int FlFile::seek(flint64 offset, SeekOrigin origin)
{
#ifndef FL_FILE_RW
    if (!m_in.fp)
        return 0;

    switch (origin) {
    case SetSeek:
        return fseek(m_in.fp, offset, SEEK_SET);
    case EndSeek:
        return fseek(m_in.fp, offset, SEEK_END);
    case CurSeek:
        return fseek(m_in.fp, offset, SEEK_CUR);
    }
#else
    if (!m_in.rw)
        return 0;

    switch (origin) {
    case SetSeek:
        return SDL_RWseek(m_in.rw, offset, RW_SEEK_SET);
    case EndSeek:
        return SDL_RWseek(m_in.rw, offset, RW_SEEK_END);
    case CurSeek:
        return SDL_RWseek(m_in.rw, offset, RW_SEEK_CUR);
    }
#endif
}

size_t FlFile::read(void *ptr, size_t size, size_t count)
{
#ifndef FL_FILE_RW
    if (!m_in.fp)
        return 0;

    return fread(ptr, size, count, m_in.fp);
#else
    if (!m_in.rw)
        return 0;

    return SDL_RWread(m_in.rw, ptr, size, count);
#endif
}

size_t FlFile::write(const void *ptr, size_t size, size_t count)
{
#ifndef FL_FILE_RW
    if (!m_in.fp)
        return 0;

    return fwrite(ptr, size, count, m_in.fp);
#else
    if (!m_in.rw)
        return 0;

    return SDL_RWwrite(m_in.rw, ptr, size, count);
#endif
}

long int FlFile::size() const
{
#ifndef FL_FILE_RW
    if (!m_in.fp)
        return 0;

    fpos_t pos;
    fgetpos(m_in.fp, &pos);
    fseek(m_in.fp, 0, SEEK_END);
    long int result = ftell(m_in.fp);
    fsetpos(m_in.fp, &pos);

    return result;
#else
    if (!m_in.rw)
        return 0;

    long int pos = SDL_RWtell(m_in.rw);
    SDL_RWseek(m_in.rw, 0, RW_SEEK_END);
    long int result = SDL_RWtell(m_in.rw);
    SDL_RWseek(m_in.rw, pos, RW_SEEK_SET);

    return result;
#endif
}

bool FlFile::readBuffer(FlBuffer *buffer)
{
    if (!buffer || !buffer->data())
        return false;

    return read(buffer->data(), buffer->size(), 1) == 1;
}

bool FlFile::canOpen(const FlString &path, FlFile::OpenMode mode)
{
    FlFile fp(path);
    bool result = fp.open(mode);
    fp.close();
    return result;
}
