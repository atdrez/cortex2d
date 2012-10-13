#include "flpvrloader.h"
#include "flfile.h"
#include "flmath.h"
#include "flbuffer.h"
#include <string.h>


FlPVRTexture::FlPVRTexture()
    : width(0),
      height(0),
      bitsPerPixel(0),
      bytesPerPixel(0),
      buffer(0),
      bufferSize(0)
{

}

FlPVRTexture::~FlPVRTexture()
{
    clear();
}

void FlPVRTexture::clear()
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
    internalFormat = 0;
}

bool FlPVRTexture::loadFromFile(const char *fileName)
{
    clear();

    FlFile fp(fileName);

    if (!fp.open(FlFile::ReadOnly)) {
        errorMessage = "Could not open PVR file";
        return false;
    }

    if (!readTexture(fp)) {
        clear();
        return false;
    }

    return true;
}

bool FlPVRTexture::readTexture(FlFile &fp)
{
    /*******************************************
     * PVR HEADER FORMAT
     *
     * OFFSET   SIZE    FORMAT   FIELD
     *
     * 0        4       UINT32   VERSION
     * 4        4       UINT32   FLAGS
     * 8        8       UINT64   PIXEL FORMAT
     * 16       4       UINT32   COLOUR SPACE
     * 20       4       UINT32   CHANNEL
     * 24       4       UINT32   HEIGHT
     * 28       4       UINT32   WIDTH
     * 32       4       UINT32   DEPTH
     * 36       4       UINT32   NUM SURFACES
     * 40       4       UINT32   NUM FACES
     * 44       4       UINT32   MIP-MAP COUNT
     * 48       4       UINT32   META DATA SIZE
     *******************************************/

    FlBuffer header(52);

    if (!fp.readBuffer(&header)) {
        errorMessage = "Could not read PVR file header";
        return false;
    }

    // version
    // 0x50565203, if endianess does match
    // 0x03525650, if endianess does not match
    const fluint32 version = header.readUInt32(0);

    // flag 0x02 is pre-multiplied by alpha
    const fluint32 flags = header.readUInt32(4);
    const bool isPreMultiplied = (flags & 0x02);

    /*******************************************
     *  PIXEL FORMAT
     *
     * 0 = PVRTC 2bpp RGB
     * 1 = PVRTC 2bpp RGBA
     * 2 = PVRTC 4bpp RGB
     * 3 = PVRTC 4bpp RGBA
     * 4 = PVRTC-II 2bpp
     * 5 = PVRTC-II 4bpp
    *******************************************/
    const fluint64 pixelFormat = header.readUInt64(8);
    const fluint32 colorSpace = header.readUInt32(16); // 0 = Linear RGB
    const fluint32 metaDataSize = header.readUInt32(48);

    width = header.readUInt32(28);
    height = header.readUInt32(24);
    bitsPerPixel = header.readUInt32(32);

    if (pixelFormat > 3 || width <= 0 || height <= 0 || bitsPerPixel <= 0) {
        errorMessage = "Invalid PVR file. Supported formats: PVRTC 2bpp RGB,"
            "PVRTC 2bpp RGBA, PVRTC 4bpp RGB, PVRTC 4bpp RGBA";
        return false;
    }

    switch (pixelFormat) {
    case 0:
        internalFormat = 0x8C01; // PVRTC_RGB_2BPPV1 = 0x8C01
        break;
    case 1:
        internalFormat = 0x8C03; // PVRTC_RGBA_2BPPV1 = 0x8C03
        break;
    case 2:
        internalFormat = 0x8C00; // PVRTC_RGB_4BPPV1 = 0x8C00
        break;
    case 3:
        internalFormat = 0x8C02; // PVRTC_RGBA_4BPPV1 = 0x8C02
        break;
    default:
        internalFormat = 0;
        break;
    }

    bytesPerPixel = (bitsPerPixel / 8);

    // 4BPP
    if (pixelFormat == 3 || pixelFormat == 4)
        bufferSize = (flMax<fluint32>(width, 8) * flMax<fluint32>(height, 8) * 4 + 7) / 8;
    else // 2BPP
        bufferSize = (flMax<fluint32>(width, 16) * flMax<fluint32>(height, 8) * 2 + 7) / 8;

    if (bufferSize <= 0) {
        errorMessage = "Invalid texture data size in PVR file";
        return false;
    }

    fp.seek(metaDataSize, FlFile::CurSeek);

    buffer = new flubyte[bufferSize];

    if (!buffer) {
        errorMessage = "Could not allocate memory for PVR texture";
        return false;
    }

    return fp.read(buffer, bufferSize, 1) == 1;
}
