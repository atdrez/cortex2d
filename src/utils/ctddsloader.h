#ifndef CTDDSLOADER_H
#define CTDDSLOADER_H

#include "ctGL.h"
#include "ctglobal.h"

class CtFile;

class CtDDSTexture
{
public:
    CtDDSTexture();
    ~CtDDSTexture();

    ctuint32 width;
    ctuint32 height;
    ctuint32 bitsPerPixel;
    ctubyte *buffer;
    ctuint32 bufferSize;
    CtString errorMessage;
    GLenum internalFormat;

    bool loadFromFile(const char *fileName);
    void clear();

private:
    bool readTexture(CtFile &fp);
};

#endif
