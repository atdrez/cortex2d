#include "cttexture.h"
#include <string.h>
#include "ctfile.h"
#include "ctopenglfunctions.h"
#include "utils/cttgaloader.h"
#include "utils/ctpngloader.h"
#include "utils/ctpvrloader.h"
#include "utils/ctddsloader.h"


CtTexture::CtTexture()
    : mId(0),
      mWidth(0),
      mHeight(0),
      mFormat(GL_RGB),
      mIsAtlas(false),
      mIsInverted(false)
{

}

CtTexture::CtTexture(bool isAtlas)
    : mId(0),
      mWidth(0),
      mHeight(0),
      mFormat(GL_RGB),
      mIsAtlas(isAtlas),
      mIsInverted(false)
{

}

CtTexture::~CtTexture()
{
    release();
}

void CtTexture::release()
{
    if (mId) {
        CtGL::glDeleteTextures(1, &mId);

        mId = 0;
        mWidth = 0;
        mHeight = 0;
        mFormat = GL_RGB;
        mIsInverted = false;
    }
}

bool CtTexture::loadFromData(int w, int h, int depth, const GLvoid *data)
{
    release();

    if (depth != 4 && depth != 3 && depth != 1)
        return false;

    CtGL::glGenTextures(1, &mId);
    CT_CHECK_GL_ERROR(return false);

    mWidth = w;
    mHeight = h;
    mIsInverted = true;
    mFormat = ((depth == 4) ? GL_RGBA : ((depth == 3) ? GL_RGB : GL_ALPHA));

    CtGL::glBindTexture(GL_TEXTURE_2D, mId);
    CT_CHECK_GL_ERROR(return false);

    CtGL::glTexImage2D(GL_TEXTURE_2D, 0, mFormat, mWidth, mHeight,
                       0, mFormat, GL_UNSIGNED_BYTE, data);
    CT_CHECK_GL_ERROR(return false);

    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void CtTexture::setError(const CtString &error)
{
    mError = error;
}

bool CtTexture::loadTGA(const CtString &fileName)
{
    CtTGATexture texture;

    if (!texture.loadFromFile(fileName.data())) {
        mError = texture.errorMessage;
        return false;
    }

    return loadFromData(texture.width, texture.height,
                        texture.bitsPerPixel / 8, texture.buffer);
}

bool CtTexture::loadPNG(const CtString &fileName)
{
    CtPNGTexture texture;

    if (!texture.loadFromFile(fileName.data())) {
        mError = texture.errorMessage;
        return false;
    }

    bool ok = loadFromData(texture.width, texture.height,
                           texture.bytesPerPixel, texture.buffer);

    if (ok)
        mIsInverted = false;

    return ok;
}

bool CtTexture::loadPVR(const CtString &fileName)
{
    CtPVRTexture texture;

    if (!texture.loadFromFile(fileName.data())) {
        mError = texture.errorMessage;
        return false;
    }

    return loadFromData(texture.width, texture.height,
                        texture.bitsPerPixel / 8, texture.buffer);
}

bool CtTexture::loadDDS(const CtString &fileName)
{
    CtDDSTexture texture;

    if (!texture.loadFromFile(fileName.data())) {
        mError = texture.errorMessage;
        return false;
    }

    return loadFromData(texture.width, texture.height,
                        texture.bitsPerPixel / 8, texture.buffer);
}

bool CtTexture::load(const CtString &fileName)
{
    const int idx = fileName.lastIndexOf('.');

    if (idx < 0)
        return false;

    const CtString &ext = fileName.substr(idx + 1);

    if (ext == "dds")
        return loadDDS(fileName);
    else if (ext == "pvr")
        return loadPVR(fileName);
    else if (ext == "png")
        return loadPNG(fileName);
    else if (ext == "tga")
        return loadTGA(fileName);

    return false;
}


CtAtlasTexture::CtAtlasTexture()
    : CtTexture(true)
{

}

CtAtlasTexture::~CtAtlasTexture()
{

}

int CtAtlasTexture::tileCount() const
{
    return mSourceRects.size();
}

int CtAtlasTexture::indexOfKey(const CtString &key) const
{
    return mKeys.value(key, -1);
}

CtRect CtAtlasTexture::sourceRectAt(int index) const
{
    if (index >= 0 && index < (int)mSourceRects.size())
        return mSourceRects.at(index);
    else
        return CtRect();
}

CtRect CtAtlasTexture::viewportRectAt(int index) const
{
    if (index >= 0 && index < (int)mViewportRects.size())
        return mViewportRects.at(index);
    else
        return CtRect();
}

bool CtAtlasTexture::loadAtlas(const CtString &filePath)
{
    CtFile fp(filePath);

    if (!fp.open(CtFile::ReadOnly))
        return false;

    ctuint32 version, magicNumber, lenName;

    if (!fp.read(&magicNumber, sizeof(ctuint32), 1) ||
        !fp.read(&version, sizeof(ctuint32), 1) ||
        !fp.read(&lenName, sizeof(ctuint32), 1))
        return false;

    if (version > 1) {
        setError("Invalid version of atlas");
        return false;
    }

    char path[lenName];
    if (fp.read(path, lenName, 1) != 1)
        return false;

    if (lenName <= 0 || lenName > 1024) {
        setError("Invalid string found on atlas file");
        return false;
    }

    bool ok = false;
    bool skipPVR = false;

    CtString extensions = (char *)CtGL::glGetString(GL_EXTENSIONS);
    extensions = " " + extensions + " ";

// XXX: check why pvr textures are not loading on simulator
#ifdef CT_IPHONE
    skipPVR = true;
#endif

    if ((extensions.contains(" GL_OES_texture_compression_S3TC ") ||
         extensions.contains(" GL_EXT_texture_compression_s3tc ")) &&
        CtFile::canOpen(filePath + ".dds", CtFile::ReadOnly)) {
        ok = load(filePath + ".dds");
    } else if (!skipPVR && extensions.contains(" GL_IMG_texture_compression_pvrtc ") &&
               CtFile::canOpen(filePath + ".pvr", CtFile::ReadOnly)) {
        ok = load(filePath + ".pvr");
    } else if (CtFile::canOpen(filePath + ".png", CtFile::ReadOnly)) {
        ok = load(filePath + ".png");
    } else if (CtFile::canOpen(filePath + ".tga", CtFile::ReadOnly)) {
        ok = load(filePath + ".tga");
    }

    if (!ok)
        return false;

    ctuint32 totalEntries;
    if (!fp.read(&totalEntries, sizeof(ctuint32), 1))
        return false;

    ctuint32 x, y, w, h, xOffset, yOffset;

    ctuint32 originalWidth, originalHeight;

    mKeys.clear();

    if (totalEntries > 0) {
        mSourceRects.resize(totalEntries);
        mViewportRects.resize(totalEntries);
    } else {
        mSourceRects.clear();
        mViewportRects.clear();
    }

    for (ctuint32 i = 0; i < totalEntries; i++) {
        ctuint32 lenKey;
        if (!fp.read(&lenKey, sizeof(ctuint32), 1))
            return false;

        if (lenKey == 0 || lenKey > 1024) {
            setError("Invalid string found on atlas file");
            return false;
        }

        char key[lenKey];
        if (!fp.read(key, lenKey, 1))
            return false;

        CtString texKey(key, lenKey);

        if (!fp.read(&x, sizeof(ctuint32), 1) ||
            !fp.read(&y, sizeof(ctuint32), 1) ||
            !fp.read(&w, sizeof(ctuint32), 1) ||
            !fp.read(&h, sizeof(ctuint32), 1) ||
            !fp.read(&xOffset, sizeof(ctuint32), 1) ||
            !fp.read(&yOffset, sizeof(ctuint32), 1) ||
            !fp.read(&originalWidth, sizeof(ctuint32), 1) ||
            !fp.read(&originalHeight, sizeof(ctuint32), 1))
            return false;

        mKeys[texKey] = i;
        mSourceRects[i] = CtRect(ctreal(x), ctreal(y),
                                 ctreal(w), ctreal(h));
        mViewportRects[i] = CtRect(-ctreal(xOffset), -ctreal(yOffset),
                                   ctreal(originalWidth), ctreal(originalHeight));
    }

    return true;
}
