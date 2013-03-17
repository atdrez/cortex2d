#ifndef CTTGALOADER_H
#define CTTGALOADER_H

#include "ctGL.h"
#include <stdio.h>
#include <stdlib.h>
#include "ctglobal.h"

class CtFile;

class CtTGATexture
{
public:
    CtTGATexture();
    ~CtTGATexture();

    ctuint16 width;
    ctuint16 height;
    ctubyte bitsPerPixel;
    ctubyte bytesPerPixel;
    ctubyte *buffer;
    ctuint32 bufferSize;
    CtString errorMessage;

    bool loadFromFile(const char *fileName);

    void clear();

private:
    bool readHeader(CtFile &fp, bool *compressed);
    bool loadFileData(CtFile &fp, bool compressed);

    CT_PRIVATE_COPY(CtTGATexture);
};

#endif
