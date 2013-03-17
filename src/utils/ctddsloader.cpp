#include "ctddsloader.h"
#include "ctfile.h"
#include "ctbuffer.h"
#include "ctmath.h"
#include <string.h>


#define COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3


CtDDSTexture::CtDDSTexture()
    : width(0),
      height(0),
      bitsPerPixel(0),
      buffer(0),
      bufferSize(0),
      errorMessage(),
      internalFormat(0)
{

}

CtDDSTexture::~CtDDSTexture()
{
    clear();
}

void CtDDSTexture::clear()
{
    if (buffer) {
        delete [] buffer;
        buffer = 0;
    }

    width = 0;
    height = 0;
    bitsPerPixel = 0;
    bufferSize = 0;
    internalFormat = 0;
}

bool CtDDSTexture::loadFromFile(const char *fileName)
{
    clear();

    CtFile fp(fileName);

    if (!fp.open(CtFile::ReadOnly)) {
        errorMessage = "Could not open PVR file";
        return false;
    }

    if (!readTexture(fp)) {
        clear();
        return false;
    }

    return true;
}

bool CtDDSTexture::readTexture(CtFile &fp)
{
    /*******************************************
     * DDS HEADER FORMAT
     *
     * OFFSET   SIZE    FORMAT   FIELD
     *
     * 0        4       UINT32   MAGIC
     * 4        4       UINT32   SIZE
     * 8        4       UINT32   FLAGS
     * 12       4       UINT32   HEIGHT
     * 16       4       UINT32   WIDTH
     * 20       4       UINT32   PITCH_OR_LINEAR_SIZE
     * 24       4       UINT32   DEPTH
     * 28       4       UINT32   MIPMAPCOUNT
     * 32      44       UBYTE    RESERVED1
     *
     * 76       4       UINT32   PIXFMT_SIZE
     * 80       4       UINT32   PIXFMT_FLAGS
     * 84       4       UINT32   PIXFMT_FOURCC
     * 88       4       UINT32   PIXFMT_RGBBITCOUNT
     * 92       4       UINT32   PIXFMT_RBITMASK
     * 96       4       UINT32   PIXFMT_GBITMASK
     * 100      4       UINT32   PIXFMT_BBITMASK
     * 104      4       UINT32   PIXFMT_ABITMASK
     *
     * 108      4       UINT32   CAPS
     * 112      4       UINT32   CAPS2
     * 116      4       UINT32   CAPS3
     * 120      4       UINT32   CAPS4
     * 124      4       UINT32   RESERVED2
     *******************************************/

    CtBuffer header(128);

    if (!fp.readBuffer(&header)) {
        errorMessage = "Could not read DDS file header";
        return false;
    }

    const ctuint32 magic = header.readUInt32(0);

    if (magic != 0x20534444) {
        errorMessage = "Invalid magic number";
        return false;
    }

    char fourCC[4];
    header.readChar(fourCC, 84, 4);

    if (strcmp(fourCC, "DXT1") != 0 &&
        strcmp(fourCC, "DXT3") != 0 &&
        strcmp(fourCC, "DXT5") != 0) {
        errorMessage = "Invalid FourCC value (Just accepts DXTn)";
        return false;
    }

    const ctuint32 dwSize = header.readUInt32(4);

    width = header.readUInt32(16);
    height = header.readUInt32(12);
    bitsPerPixel = header.readUInt32(88);

    bitsPerPixel = 32; // XXX

    if ((width & (width - 1)) || (height & (height - 1))) {
        errorMessage = "Texture size should be power of two";
        return false;
    }

    if (dwSize != 124 || width <= 0 || height <= 0 || (bitsPerPixel != 24 && bitsPerPixel != 32)) {
        errorMessage = "Invalid DDS file";
        return false;
    }

    ctuint32 divSize = 4;
    ctuint32 blockBytes = 8;

    switch (fourCC[3]) {
    case '1':
        blockBytes = 8;
        if (bitsPerPixel == 24)
            internalFormat = COMPRESSED_RGB_S3TC_DXT1_EXT;
        else
            internalFormat = COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case '3':
        blockBytes = 16;
        internalFormat = COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case '5':
        blockBytes = 16;
        internalFormat = COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        internalFormat = 0;
        break;
    }

    bufferSize = ctMax(divSize, width) / divSize * ctMax(divSize, height) / divSize * blockBytes;

    if (bufferSize <= 0) {
        errorMessage = "Invalid texture data size in DDS file";
        return false;
    }

    buffer = new ctubyte[bufferSize];

    if (!buffer) {
        errorMessage = "Could not allocate memory for PVR texture";
        return false;
    }

    return fp.read(buffer, bufferSize, 1) == 1;
}
