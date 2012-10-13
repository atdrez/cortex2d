#ifndef FLPVRLOADER_H
#define FLPVRLOADER_H

#include "flGL.h"
#include "flglobal.h"

class FlFile;

class FlPVRTexture
{
public:
    FlPVRTexture();
    ~FlPVRTexture();

    fluint32 width;
    fluint32 height;
    fluint32 bitsPerPixel;
    fluint32 bytesPerPixel;
    flubyte *buffer;
    fluint32 bufferSize;
    FlString errorMessage;
    GLenum internalFormat;

    bool loadFromFile(const char *fileName);
    void clear();

private:
    bool readTexture(FlFile &fp);
};

#endif
