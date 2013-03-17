#ifndef CTPNGLOADER_H
#define CTPNGLOADER_H

#include "ctGL.h"
#include <stdio.h>
#include <stdlib.h>
#include "ctglobal.h"

class CtFile;

class CtPNGTexture
{
public:
    CtPNGTexture();
    ~CtPNGTexture();

    ctuint32 width;
    ctuint32 height;
    ctubyte bytesPerPixel;
    ctubyte *buffer;
    ctuint32 bufferSize;
    CtString errorMessage;

    bool loadFromFile(const char *fileName);

    void clear();

private:
#ifndef CT_IPHONE
    bool loadFileData(CtFile &fp);
#endif

    CT_PRIVATE_COPY(CtPNGTexture);
};



#endif
