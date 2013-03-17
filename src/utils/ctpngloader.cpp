#include "ctpngloader.h"
#include "png.h"
#include "ctfile.h"
#include "ctbuffer.h"
#include <string.h>

typedef struct {
    unsigned char *data;
    int size;
    int offset;
} ct_PngData;

static void ct_pngReadDataCallback(png_structp pngPtr, png_bytep data, png_size_t length)
{
    ct_PngData *d = reinterpret_cast<ct_PngData *>(png_get_io_ptr(pngPtr));

    if (d->offset + int(length) > d->size) {
        png_error(pngPtr, "Error on png read callback");
        return;
    }

    memcpy(data, d->data + d->offset, length);
    d->offset += length;
}


CtPNGTexture::CtPNGTexture()
    : width(0),
      height(0),
      bytesPerPixel(0),
      buffer(0),
      bufferSize(0)
{

}

CtPNGTexture::~CtPNGTexture()
{
    clear();
}

void CtPNGTexture::clear()
{
    if (buffer) {
        delete [] buffer;
        buffer = 0;
    }

    width = 0;
    height = 0;
    bytesPerPixel = 0;
    bufferSize = 0;
}

bool CtPNGTexture::loadFromFile(const char *fileName)
{
    clear();

    CtFile fp(fileName);

    if (!fp.open(CtFile::ReadOnly)) {
        errorMessage = "Could not open PNG file";
        return false;
    }

    bool result = loadFileData(fp);

    if (!result)
        clear();

    fp.close();

    return result;
}

bool CtPNGTexture::loadFileData(CtFile &fp)
{
    CtBuffer header(8);

    if (!fp.readBuffer(&header)) {
        errorMessage = "Unable to read PNG header";
        return false;
    }

    // check png header
    if (png_sig_cmp(header.data(), 0, 8)) {
        errorMessage = "Invalid PNG header signature";
        return false;
    }

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if (!pngPtr) {
        errorMessage = "Unable to create PNG read struct";
        return false;
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);

    if (!infoPtr) {
        errorMessage = "Unable to create PNG info struct";
        png_destroy_read_struct(&pngPtr, 0, 0);
        return false;
    }

    if (setjmp(png_jmpbuf(pngPtr))) {
        errorMessage = "Unable to set libpng jmp";
        png_destroy_read_struct(&pngPtr, &infoPtr, 0);
        return false;
    }

    const int fileSize = (int)fp.size();

    CtBuffer content(fileSize);

    fp.seek(0, CtFile::SetSeek);

    if (!fp.readBuffer(&content)) {
        errorMessage = "Unable to read PNG file content";
        png_destroy_read_struct(&pngPtr, &infoPtr, 0);
        return false;
    }

    // set the read call back function
    ct_PngData pngData;
    pngData.offset = 0;
    pngData.size = fileSize;
    pngData.data = content.data();

    png_set_read_fn(pngPtr, &pngData, ct_pngReadDataCallback);

    png_read_png(pngPtr, infoPtr, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND |
                 PNG_TRANSFORM_GRAY_TO_RGB | PNG_TRANSFORM_STRIP_16, 0);

    // image info
    const png_uint_32 w = infoPtr->width;
    const png_uint_32 h = infoPtr->height;
    const bool containsAlpha = bool(infoPtr->color_type & PNG_COLOR_MASK_ALPHA);

    bytesPerPixel = containsAlpha ? 4 : 3;

    bufferSize = h * w * bytesPerPixel;
    buffer = new unsigned char[bufferSize];

    if (!buffer) {
        errorMessage = "Unable to allocate image buffer";
        png_destroy_read_struct(&pngPtr, &infoPtr, 0);
        return false;
    }

    const png_bytep *rows = png_get_rows(pngPtr, infoPtr);
    const unsigned int bytesPerRow = w * bytesPerPixel;

    for (png_uint_32 i = 0; i < h; i++)
        memcpy(buffer + bytesPerRow * i, rows[i], bytesPerRow);

    // set image size
    width = (ctuint16)w;
    height = (ctuint16)h;

    // destroy png info
    png_destroy_read_struct(&pngPtr, &infoPtr, 0);

    return true;
}
