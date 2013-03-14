#include "cttexture.h"
#include "ctopenglfunctions.h"
#include "utils/ctfile.h"
#include "utils/cttgaloader.h"
#include "utils/ctpngloader.h"
#include "utils/ctpvrloader.h"
#include "utils/ctddsloader.h"
#include <string.h>


CtString ct_dirPath(const CtString &path)
{
    CtString r = path;
    int idx = r.lastIndexOf('/');

    if (idx < 0) {
        return CtString("");
    } else if (idx == 0) {
        return CtString("/");
    } else {
        r.remove(idx, r.length());
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

bool CtTexture::loadFromData(int w, int h, int depth, const GLvoid *data)
{
    release();

    if (depth != 4 && depth != 3 && depth != 1)
        return false;

    CtGL::glGenTextures(1, &m_textureId);
    CT_CHECK_GL_ERROR(return false);

    m_width = w;
    m_height = h;
    m_format = ((depth == 4) ? GL_RGBA : ((depth == 3) ? GL_RGB : GL_ALPHA));

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

    if (!texture.loadFromFile((char *)fileName.data())) {
        m_error = texture.errorMessage;
        return false;
    }

    return loadFromData(texture.width, texture.height,
                        texture.bitsPerPixel / 8, texture.buffer);
}

bool CtTexture::loadPNG(const CtString &fileName)
{
    CtPNGTexture texture;

    if (!texture.loadFromFile((char *)fileName.data())) {
        m_error = texture.errorMessage;
        return false;
    }

    bool ok = loadFromData(texture.width, texture.height,
                           texture.bytesPerPixel, texture.buffer);

    if (ok)
        m_inverted = false;

    return ok;
}

bool CtTexture::loadPVR(const CtString &fileName)
{
    CtPVRTexture texture;
    if (!texture.loadFromFile((char *)fileName.data())) {
        m_error = texture.errorMessage;
        return false;
    }

    return loadFromData(texture.width, texture.height,
                        texture.bitsPerPixel / 8, texture.buffer);
}

bool CtTexture::loadDDS(const CtString &fileName)
{
    CtDDSTexture texture;
    if (!texture.loadFromFile((char *)fileName.data())) {
        m_error = texture.errorMessage;
        CT_DEBUG(m_error);
        return false;
    }

    return loadFromData(texture.width, texture.height,
                        texture.bitsPerPixel / 8, texture.buffer);
}

bool CtTexture::load(const CtString &fileName)
{
    int idx = fileName.lastIndexOf('.');

    if (idx < 0)
        return false;

    const CtString &ext = fileName.substr(idx + 1);

    if (ext == "dds") {
        return loadDDS(fileName);
    } else if (ext == "pvr") {
        return loadPVR(fileName);
    } else if (ext == "png") {
        return loadPNG(fileName);
    } else if (ext == "tga") {
        return loadTGA(fileName);
    }

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
        ok = load(filePath + ".dds");
    } else if (!skipPVR && extensions.contains(" GL_IMG_texture_compression_pvrtc ") &&
               CtFile::canOpen(filePath + ".pvr", CtFile::ReadOnly)) {
        ok = load(filePath + ".pvr");
    } else if (CtFile::canOpen(filePath + ".png", CtFile::ReadOnly)) {
        ok = load(filePath + ".png");
    } else if (CtFile::canOpen(filePath + ".tga", CtFile::ReadOnly)) {
        ok = load(filePath + ".tga");
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
        setSourceRectAt(i, CtRect(ctreal(x), ctreal(y), ctreal(w), ctreal(h)));
        setViewportRectAt(i, CtRect(-ctreal(xOffset), -ctreal(yOffset),
                                    ctreal(originalWidth), ctreal(originalHeight)));
    }

    return true;
}

int CtAtlasTexture::indexOfKey(const CtString &key) const
{
    return m_keys.value(key, -1);
}


CtTextureCache::CtTextureCache()
{

}

CtTextureCache::~CtTextureCache()
{

}

CtTextureCache *CtTextureCache::instance()
{
    static CtTextureCache *result = 0;

    if (!result)
        result = new CtTextureCache();

    return result;
}

void CtTextureCache::insert(const CtString &key, CtTexture *texture)
{
    CtTexture *oldTexture = CtTextureCache::find(key);

    if (oldTexture)
        delete oldTexture;

    CtTextureCache *d = instance();
    d->m_map[key] = texture;
}

CtTexture *CtTextureCache::take(const CtString &key)
{
    CtTextureCache *d = instance();
    CtMap<CtString, CtTexture *>::iterator it = d->m_map.find(key);

    if (it == d->m_map.end())
        return 0;

    CtTexture *result = it->second;
    d->m_map.erase(it);
    return result;
}

void CtTextureCache::remove(const CtString &key)
{
    CtTexture *texture = CtTextureCache::take(key);

    if (texture)
        delete texture;
}

CtTexture *CtTextureCache::find(const CtString &key)
{
    CtTextureCache *d = instance();
    CtMap<CtString, CtTexture *>::const_iterator it = d->m_map.find(key);
    return (it == d->m_map.end()) ? 0 : it->second;
}

void CtTextureCache::clear()
{
    CtTextureCache *d = instance();
    CtMap<CtString, CtTexture *>::iterator it;

    for (it = d->m_map.begin(); it != d->m_map.end(); it++)
        delete it->second;

    d->m_map.clear();
}
