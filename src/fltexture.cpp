#include "fltexture.h"
#include "flopenglfunctions.h"
#include "utils/flfile.h"
#include "utils/fltgaloader.h"
#include "utils/flpvrloader.h"
#include "utils/flddsloader.h"
#include <string.h>


FlString fl_dirPath(const FlString &path)
{
    FlString r = path;
    int idx = r.find_last_of("/");

    if (idx < 0) {
        return FlString("");
    } else if (idx == 0) {
        return FlString("/");
    } else {
        r.erase(idx, r.length());
        return r;
    }
}


FlTexture::FlTexture()
    : m_inverted(false),
      m_textureId(0),
      m_width(0),
      m_height(0),
      m_format(GL_RGB),
      m_isAtlas(false)
{

}

FlTexture::FlTexture(bool isAtlas)
    : m_inverted(false),
      m_textureId(0),
      m_width(0),
      m_height(0),
      m_format(GL_RGB),
      m_isAtlas(isAtlas)
{

}

FlTexture::~FlTexture()
{

}

void FlTexture::release()
{
    if (m_textureId) {
        FlGL::glDeleteTextures(1, &m_textureId);
        m_width = 0;
        m_height = 0;
        m_textureId = 0;
        m_inverted = false;
        m_format = GL_RGB;
    }
}

bool FlTexture::loadTGA(const FlString &fileName)
{
    release();

    FlTGATexture texture;

    if (!texture.loadFromFile((char *)fileName.c_str())) {
        m_error = texture.errorMessage;
        return false;
    }

    FlGL::glGenTextures(1, &m_textureId);
    FL_CHECK_GL_ERROR(return false);

    m_width = texture.width;
    m_height = texture.height;
    m_format = (texture.bitsPerPixel == 24) ? GL_RGB : GL_RGBA;

    FlGL::glBindTexture(GL_TEXTURE_2D, m_textureId);
    FL_CHECK_GL_ERROR(return false);

    FlGL::glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width,
                       m_height, 0, m_format, GL_UNSIGNED_BYTE, texture.buffer);
    FL_CHECK_GL_ERROR(return false);

    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_inverted = true;
    return true;
}

bool FlTexture::loadPVR(const FlString &fileName)
{
    release();

    FlPVRTexture texture;
    if (!texture.loadFromFile((char *)fileName.c_str())) {
        m_error = texture.errorMessage;
        return false;
    }

    FlGL::glGenTextures(1, &m_textureId);
    FL_CHECK_GL_ERROR(return false);

    FlGL::glBindTexture(GL_TEXTURE_2D, m_textureId);
    FL_CHECK_GL_ERROR(return false);

    m_width = texture.width;
    m_height = texture.height;
    m_format = (texture.bitsPerPixel == 24) ? GL_RGB : GL_RGBA;

    FlGL::glCompressedTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat,
                                 m_width, m_height, 0, texture.bufferSize, texture.buffer);
    FL_CHECK_GL_ERROR(return false);

    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_inverted = true;
    return true;
}

bool FlTexture::loadDDS(const FlString &fileName)
{
    release();

    FlDDSTexture texture;
    if (!texture.loadFromFile((char *)fileName.c_str())) {
        m_error = texture.errorMessage;
        FL_DEBUG(m_error);
        return false;
    }

    FlGL::glGenTextures(1, &m_textureId);
    FL_CHECK_GL_ERROR(return false);

    FlGL::glBindTexture(GL_TEXTURE_2D, m_textureId);
    FL_CHECK_GL_ERROR(return false);

    m_width = texture.width;
    m_height = texture.height;
    m_format = (texture.bitsPerPixel == 24) ? GL_RGB : GL_RGBA;

    FlGL::glCompressedTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat,
                                 m_width, m_height, 0, texture.bufferSize, texture.buffer);
    FL_CHECK_GL_ERROR(return false);

    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    FlGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_inverted = false;
    return true;
}

FlAtlasTexture::FlAtlasTexture()
    : FlTexture(true)
{

}

FlAtlasTexture::~FlAtlasTexture()
{

}

int FlAtlasTexture::tileCount() const
{
    return m_rects.size();
}

void FlAtlasTexture::setTileCount(int count)
{
    if (count > 0) {
        m_rects.resize(count);
        m_drects.resize(count);
    }
}

FlRect FlAtlasTexture::sourceRectAt(int index) const
{
    if (index >= 0 && index < (int)m_rects.size())
        return m_rects.at(index);
    else
        return FlRect();
}

void FlAtlasTexture::setSourceRectAt(int index, const FlRect &rect)
{
    if (index >= 0 && index < (int)m_rects.size())
        m_rects[index] = rect;
}

FlRect FlAtlasTexture::viewportRectAt(int index) const
{
    if (index >= 0 && index < (int)m_drects.size())
        return m_drects.at(index);
    else
        return FlRect();
}

void FlAtlasTexture::setViewportRectAt(int index, const FlRect &rect)
{
    if (index >= 0 && index < (int)m_drects.size())
        m_drects[index] = rect;
}

bool FlAtlasTexture::loadAtlas(const FlString &filePath)
{
    FlFile fp(filePath);

    if (!fp.open(FlFile::ReadOnly))
        return false;

    fluint32 version, magicNumber, lenName;

    if (!fp.read(&magicNumber, sizeof(fluint32), 1) ||
        !fp.read(&version, sizeof(fluint32), 1) ||
        !fp.read(&lenName, sizeof(fluint32), 1))
        return false;

    if (version > 1) {
        FL_WARNING("Invalid version of atlas");
        return false;
    }

    char path[lenName];
    if (fp.read(path, lenName, 1) != 1)
        return false;

    if (lenName <= 0 || lenName > 1024) {
        FL_WARNING("Invalid string found on atlas file");
        return false;
    }

    bool ok = false;
    bool skipPVR = false;

    FlString extensions = (char *)FlGL::glGetString(GL_EXTENSIONS);
    extensions = " " + extensions + " ";

// XXX: check why pvr textures are not loading on simulator
#ifdef FL_IPHONE
    skipPVR = true;
#endif

    if ((extensions.contains(" GL_OES_texture_compression_S3TC ") ||
         extensions.contains(" GL_EXT_texture_compression_s3tc ")) &&
        FlFile::canOpen(filePath + ".dds", FlFile::ReadOnly)) {
        ok = loadDDS(filePath + ".dds");
    } else if (!skipPVR && extensions.contains(" GL_IMG_texture_compression_pvrtc ") &&
               FlFile::canOpen(filePath + ".pvr", FlFile::ReadOnly)) {
        ok = loadPVR(filePath + ".pvr");
    } else if (FlFile::canOpen(filePath + ".tga", FlFile::ReadOnly)) {
        ok = loadTGA(filePath + ".tga");
    }

    if (!ok) {
        FL_WARNING("Unable to load atlas file " << error());
        return false;
    }

    fluint32 totalEntries;
    if (!fp.read(&totalEntries, sizeof(fluint32), 1))
        return false;

    fluint32 x, y, w, h, xOffset, yOffset;
    fluint32 originalWidth, originalHeight;

    setTileCount(totalEntries);
    m_keys.clear();

    for (fluint32 i = 0; i < totalEntries; i++) {
        fluint32 lenKey;
        if (!fp.read(&lenKey, sizeof(fluint32), 1))
            return false;

        if (lenKey == 0 || lenKey > 1024) {
            FL_WARNING("Invalid string found on atlas file");
            return false;
        }

        char key[lenKey];
        if (!fp.read(key, lenKey, 1))
            return false;

        FlString texKey(key, lenKey);

        if (!fp.read(&x, sizeof(fluint32), 1) ||
            !fp.read(&y, sizeof(fluint32), 1) ||
            !fp.read(&w, sizeof(fluint32), 1) ||
            !fp.read(&h, sizeof(fluint32), 1) ||
            !fp.read(&xOffset, sizeof(fluint32), 1) ||
            !fp.read(&yOffset, sizeof(fluint32), 1) ||
            !fp.read(&originalWidth, sizeof(fluint32), 1) ||
            !fp.read(&originalHeight, sizeof(fluint32), 1))
            return false;

        m_keys[texKey] = i;
        setSourceRectAt(i, FlRect(x, y, w, h));
        setViewportRectAt(i, FlRect(-xOffset, -yOffset, originalWidth, originalHeight));
    }

    return true;
}

int FlAtlasTexture::indexOfKey(const FlString &key) const
{
    return m_keys.value(key, -1);
}
