#ifndef FLFILE_H
#define FLFILE_H

#include <stdio.h>
#include <stdlib.h>
#include "flglobal.h"

class FlBuffer;
class SDL_RWops;

class FlFile
{
public:
    enum SeekOrigin {
        SetSeek,
        CurSeek,
        EndSeek,
    };

    enum OpenMode {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

    FlFile(const FlString &path);
    ~FlFile();

    bool open(OpenMode mode);
    void close();

    int seek(flint64 offset, SeekOrigin origin);
    size_t read(void *ptr, size_t size, size_t count);
    size_t write(const void *ptr, size_t size, size_t count);

    bool readBuffer(FlBuffer *buffer);

    long int size() const;

    static bool canOpen(const FlString &path, FlFile::OpenMode mode);

private:
    union {
        FILE *fp;
        SDL_RWops *rw;
    } m_in;

    FlString m_path;
};



#endif
