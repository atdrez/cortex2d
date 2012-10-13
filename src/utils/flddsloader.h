#ifndef FLDDSLOADER_H
#define FLDDSLOADER_H

#include "flGL.h"
#include "flglobal.h"

class FlFile;

class FlDDSTexture
{
public:
    FlDDSTexture();
    ~FlDDSTexture();

    fluint32 width;
    fluint32 height;
    fluint32 bitsPerPixel;
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
