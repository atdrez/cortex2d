#include "cttgaloader.h"
#include "ctfile.h"
#include "ctbuffer.h"
#include <string.h>

static inline void convertTGAPixel(unsigned char *pixel, int byteCount)
{
    // bit-format: BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA
    if (byteCount == 3 || byteCount == 4) {
        unsigned char c = pixel[0];
        pixel[0] = pixel[2];
        pixel[2] = c;
    }
}


CtTGATexture::CtTGATexture()
    : width(0),
      height(0),
      bitsPerPixel(0),
      bytesPerPixel(0),
      buffer(0),
      bufferSize(0),
      errorMessage()
{

}

CtTGATexture::~CtTGATexture()
{
    clear();
}

void CtTGATexture::clear()
{
    if (buffer) {
        delete [] buffer;
        buffer = 0;
    }

    width = 0;
    height = 0;
    bitsPerPixel = 0;
    bytesPerPixel = 0;
    bufferSize = 0;
}

bool CtTGATexture::loadFromFile(const char *fileName)
{
    clear();

    CtFile fp(fileName);

    if (!fp.open(CtFile::ReadOnly)) {
        errorMessage = "Could not open TGA file";
        return false;
    }

    bool compressed;
    if (!readHeader(fp, &compressed)) {
        fp.close();
        clear();
        return false;
    }

    buffer = new unsigned char[bufferSize];

    if (!buffer) {
        errorMessage = "Could not allocate memory for TGA image";
        fp.close();
        clear();
        return false;
    }

    bool result = loadFileData(fp, compressed);

    if (!result)
        clear();

    fp.close();

    return result;
}

bool CtTGATexture::readHeader(CtFile &fp, bool *compressed)
{
    /*******************************************
     * TGA HEADER FORMAT
     *
     * OFFSET   BYTES    FIELD
     *
     * 0        1        id length
     * 1        1        colourmap type
     * 2        1        datatype code
     * 3        2        colourmap origin
     * 5        2        colourmap length
     * 7        1        colourmap depth
     * 8        2        x origin
     * 10       2        y origin
     * 12       2        width
     * 14       2        height
     * 16       1        bits per pixel
     * 17       1        image descriptor
    *******************************************/

    CtBuffer header(18);

    if (!fp.readBuffer(&header)) {
        errorMessage = "Could not read TGA file header";
        return false;
    }

    ctubyte idLength = header.readUByte(0);
    ctubyte colorMapType = header.readUByte(1);
    ctubyte dataTypeCode = header.readUByte(2);

    if ((dataTypeCode != 2 && dataTypeCode != 10) || colorMapType != 0) {
        errorMessage = "Invalid TGA file: Only types 2 and 10 are supported without using colormap";
        return false;
    }

    // skip identification field
    fp.seek(idLength, CtFile::CurSeek);

    width = header.readUInt16(12);
    height = header.readUInt16(14);
    bitsPerPixel = header.readUByte(16);

    // check if information is valid
    if (width <= 0 || height <= 0 || (bitsPerPixel != 24 && bitsPerPixel !=32)) {
        errorMessage = "Invalid TGA texture: Only supports images with 24 or 32 bpp";
        return false;
    }

    *compressed = (dataTypeCode == 10);
    bytesPerPixel = (bitsPerPixel / 8);
    bufferSize = (bytesPerPixel * width * height);

    return true;
}

bool CtTGATexture::loadFileData(CtFile &fp, bool compressed)
{
    if (!compressed) {
        if (fp.read(buffer, 1, bufferSize) != bufferSize) {
            errorMessage = "Unexpected error while reading TGA file";
            return false;
        }

        for (ctuint32 i = 0; i < bufferSize; i += bytesPerPixel)
            convertTGAPixel(&buffer[i], bytesPerPixel);

        return true;
    }

    for (ctuint32 i = 0; i < bufferSize; ) {
        ctubyte packetHeader;
        if (fp.read(&packetHeader, sizeof(packetHeader), 1) != 1) {
            errorMessage = "Unexpected error while reading TGA file";
            return false;
        }

        // retrieve info from header
        bool rawPacket = !(packetHeader & 128); // 1-bit (packet type: 0=RAW, 1=RLE)
        ctuint32 packetCount = (packetHeader & 127) + 1; // 7-bits (number of packets)
        ctuint32 packetFinalPos = (i + bytesPerPixel * packetCount);

        if (packetFinalPos > bufferSize) {
            errorMessage = "Too many pixels provided in TGA image";
            return false;
        }

        if (rawPacket) {
            // Raw packets provides N pixels (N == packetCount)
            if (fp.read(&buffer[i], bytesPerPixel, packetCount) != packetCount) {
                errorMessage = "Unexpected error while reading TGA file";
                return false;
            }

            for (; i < packetFinalPos; i += bytesPerPixel)
                convertTGAPixel(&buffer[i], bytesPerPixel);
        } else {
            // RLE packets provides just 1 pixel
            ctubyte pixel[bytesPerPixel];
            if (fp.read(pixel, bytesPerPixel, 1) != 1) {
                errorMessage = "Unexpected error while reading TGA file";
                return false;
            }

            convertTGAPixel(pixel, bytesPerPixel);

            // replicate pixel N times (N == packetCount)
            for (; i < packetFinalPos; i += bytesPerPixel)
                memcpy(&buffer[i], pixel, bytesPerPixel);
        }
    }

    return true;
}
