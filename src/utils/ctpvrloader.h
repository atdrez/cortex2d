#ifndef CTPVRLOADER_H
#define CTPVRLOADER_H

#include "ctGL.h"
#include "ctglobal.h"

class CtFile;

class CtPVRTexture
{
public:
    CtPVRTexture();
    ~CtPVRTexture();

    ctuint32 width;
    ctuint32 height;
    ctuint32 bitsPerPixel;
    ctuint32 bytesPerPixel;
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
