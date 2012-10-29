#include "cttexture.h"
#include "ctopenglfunctions.h"
#include "utils/ctfile.h"
#include "utils/cttgaloader.h"
#include "utils/ctpvrloader.h"
#include "utils/ctddsloader.h"
#include <string.h>


CtString ct_dirPath(const CtString &path)
{
    CtString r = path;
    int idx = r.find_last_of("/");

    if (idx < 0) {
        return CtString("");
    } else if (idx == 0) {
        return CtString("/");
    } else {
        r.erase(idx, r.length());
        return r;
    }
}


CtTexture::CtTexture()
    : m_inverted(false),
      m_textureId(0),
      m_width(0),
      m_height(0),
      m_format(GL_RGB),
      m_isAtlas(false)
{

}

CtTexture::CtTexture(bool isAtlas)
    : m_inverted(false),
      m_textureId(0),
      m_width(0),
      m_height(0),
      m_format(GL_RGB),
      m_isAtlas(isAtlas)
{

}

CtTexture::~CtTexture()
{

}

void CtTexture::release()
{
    if (m_textureId) {
        CtGL::glDeleteTextures(1, &m_textureId);
        m_width = 0;
        m_height = 0;
        m_textureId = 0;
        m_inverted = false;
        m_format = GL_RGB;
    }
}

bool CtTexture::reset(int w, int h, bool alpha, const GLvoid *data)
{
    release();

    CtGL::glGenTextures(1, &m_textureId);
    CT_CHECK_GL_ERROR(return false);

    m_width = w;
    m_height = h;
    m_format = alpha ? GL_RGBA : GL_RGB;

    CtGL::glBindTexture(GL_TEXTURE_2D, m_textureId);
    CT_CHECK_GL_ERROR(return false);

    CtGL::glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width,
                       m_height, 0, m_format, GL_UNSIGNED_BYTE, data);

    CT_CHECK_GL_ERROR(return false);

    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_inverted = true;
    return true;
}

bool CtTexture::loadTGA(const CtString &fileName)
{
    CtTGATexture texture;

    if (!texture.loadFromFile((char *)fileName.c_str())) {
        m_error = texture.errorMessage;
        return false;
    }

    return reset(texture.width, texture.height,
                 texture.bitsPerPixel == 32, texture.buffer);
}

bool CtTexture::loadPVR(const CtString &fileName)
{
    CtPVRTexture texture;
    if (!texture.loadFromFile((char *)fileName.c_str())) {
        m_error = texture.errorMessage;
        return false;
    }

    return reset(texture.width, texture.height,
                 texture.bitsPerPixel == 32, texture.buffer);
}

bool CtTexture::loadDDS(const CtString &fileName)
{
    CtDDSTexture texture;
    if (!texture.loadFromFile((char *)fileName.c_str())) {
        m_error = texture.errorMessage;
        CT_DEBUG(m_error);
        return false;
    }

    return reset(texture.width, texture.height,
                 texture.bitsPerPixel == 32, texture.buffer);
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
    return m_rects.size();
}

void CtAtlasTexture::setTileCount(int count)
{
    if (count > 0) {
        m_rects.resize(count);
        m_drects.resize(count);
    }
}

CtRect CtAtlasTexture::sourceRectAt(int index) const
{
    if (index >= 0 && index < (int)m_rects.size())
        return m_rects.at(index);
    else
        return CtRect();
}

void CtAtlasTexture::setSourceRectAt(int index, const CtRect &rect)
{
    if (index >= 0 && index < (int)m_rects.size())
        m_rects[index] = rect;
}

CtRect CtAtlasTexture::viewportRectAt(int index) const
{
    if (index >= 0 && index < (int)m_drects.size())
        return m_drects.at(index);
    else
        return CtRect();
}

void CtAtlasTexture::setViewportRectAt(int index, const CtRect &rect)
{
    if (index >= 0 && index < (int)m_drects.size())
        m_drects[index] = rect;
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
        CT_WARNING("Invalid version of atlas");
        return false;
    }

    char path[lenName];
    if (fp.read(path, lenName, 1) != 1)
        return false;

    if (lenName <= 0 || lenName > 1024) {
        CT_WARNING("Invalid string found on atlas file");
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
        ok = loadDDS(filePath + ".dds");
    } else if (!skipPVR && extensions.contains(" GL_IMG_texture_compression_pvrtc ") &&
               CtFile::canOpen(filePath + ".pvr", CtFile::ReadOnly)) {
        ok = loadPVR(filePath + ".pvr");
    } else if (CtFile::canOpen(filePath + ".tga", CtFile::ReadOnly)) {
        ok = loadTGA(filePath + ".tga");
    }

    if (!ok) {
        CT_WARNING("Unable to load atlas file " << error());
        return false;
    }

    ctuint32 totalEntries;
    if (!fp.read(&totalEntries, sizeof(ctuint32), 1))
        return false;

    ctuint32 x, y, w, h, xOffset, yOffset;
    ctuint32 originalWidth, originalHeight;

    setTileCount(totalEntries);
    m_keys.clear();

    for (ctuint32 i = 0; i < totalEntries; i++) {
        ctuint32 lenKey;
        if (!fp.read(&lenKey, sizeof(ctuint32), 1))
            return false;

        if (lenKey == 0 || lenKey > 1024) {
            CT_WARNING("Invalid string found on atlas file");
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

        m_keys[texKey] = i;
        setSourceRectAt(i, CtRect(x, y, w, h));
        setViewportRectAt(i, CtRect(-xOffset, -yOffset, originalWidth, originalHeight));
    }

    return true;
}

int CtAtlasTexture::indexOfKey(const CtString &key) const
{
    return m_keys.value(key, -1);
}
