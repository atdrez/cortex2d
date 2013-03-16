#include "ctfile.h"
#include "ctbuffer.h"

#if defined(CT_ANDROID)
#    include "SDL.h"
#    define CT_FILE_RW
#endif


CtFile::CtFile(const CtString &path)
    : m_path(path)
{
    m_in.fp = 0;
    m_in.rw = 0;
}

CtFile::~CtFile()
{
    close();
}

bool CtFile::open(OpenMode mode)
{
    close();

#ifndef CT_FILE_RW
    switch (mode) {
    case ReadOnly:
        m_in.fp = fopen(m_path.data(), "r");
        break;
    case WriteOnly:
        m_in.fp = fopen(m_path.data(), "w");
        break;
    default:
        m_in.fp = fopen(m_path.data(), "rw");
        break;
    }
    return m_in.fp;
#else
    switch (mode) {
    case ReadOnly:
        m_in.rw = SDL_RWFromFile(m_path.data(), "r");
        break;
    case WriteOnly:
        m_in.rw = SDL_RWFromFile(m_path.data(), "w");
        break;
    default:
        m_in.rw = SDL_RWFromFile(m_path.data(), "rw");
        break;
    }
    return m_in.rw;
#endif
}

void CtFile::close()
{
#ifndef CT_FILE_RW
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

int CtFile::seek(ctint64 offset, SeekOrigin origin)
{
#ifndef CT_FILE_RW
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

size_t CtFile::read(void *ptr, size_t size, size_t count)
{
#ifndef CT_FILE_RW
    if (!m_in.fp)
        return 0;

    return fread(ptr, size, count, m_in.fp);
#else
    if (!m_in.rw)
        return 0;

    return SDL_RWread(m_in.rw, ptr, size, count);
#endif
}

size_t CtFile::write(const void *ptr, size_t size, size_t count)
{
#ifndef CT_FILE_RW
    if (!m_in.fp)
        return 0;

    return fwrite(ptr, size, count, m_in.fp);
#else
    if (!m_in.rw)
        return 0;

    return SDL_RWwrite(m_in.rw, ptr, size, count);
#endif
}

long int CtFile::size() const
{
#ifndef CT_FILE_RW
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

bool CtFile::readBuffer(CtBuffer *buffer)
{
    if (!buffer || !buffer->data())
        return false;

    return read(buffer->data(), buffer->size(), 1) == 1;
}

bool CtFile::canOpen(const CtString &path, CtFile::OpenMode mode)
{
    CtFile fp(path);
    bool result = fp.open(mode);
    fp.close();
    return result;
}
