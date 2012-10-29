#ifndef CTFILE_H
#define CTFILE_H

#include <stdio.h>
#include <stdlib.h>
#include "ctglobal.h"

class CtBuffer;
class SDL_RWops;

class CtFile
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

    CtFile(const CtString &path);
    ~CtFile();

    bool open(OpenMode mode);
    void close();

    int seek(ctint64 offset, SeekOrigin origin);
    size_t read(void *ptr, size_t size, size_t count);
    size_t write(const void *ptr, size_t size, size_t count);

    bool readBuffer(CtBuffer *buffer);

    long int size() const;

    static bool canOpen(const CtString &path, CtFile::OpenMode mode);

private:
    union {
        FILE *fp;
        SDL_RWops *rw;
    } m_in;

    CtString m_path;
};



#endif
