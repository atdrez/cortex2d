#ifndef FLTGALOADER_H
#define FLTGALOADER_H

#include "flGL.h"
#include <stdio.h>
#include <stdlib.h>
#include "flglobal.h"

class FlFile;

class FlTGATexture
{
public:
    FlTGATexture();
    ~FlTGATexture();

    fluint16 width;
    fluint16 height;
    flubyte bitsPerPixel;
    flubyte bytesPerPixel;
    flubyte *buffer;
    fluint32 bufferSize;
    FlString errorMessage;

    bool loadFromFile(const char *fileName);

    void clear();

private:
    bool readHeader(FlFile &fp, bool *compressed);
    bool loadFileData(FlFile &fp, bool compressed);
};



#endif
