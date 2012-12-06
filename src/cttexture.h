#ifndef CTTEXTURE_H
#define CTTEXTURE_H

#include "ctglobal.h"
#include "ctGL.h"
#include "ctrect.h"
#include "ctpoint.h"
#include "ctmap.h"
#include "ctvector.h"

class CtTexture
{
public:
    CtTexture();
    virtual ~CtTexture();

    GLuint id() const { return m_textureId; }
    GLenum format() const { return m_format; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    bool isAtlas() const { return m_isAtlas; }
    bool isInverted() const { return m_inverted; }

    CtString error() const { return m_error; }
    bool isValid() const { return m_textureId != 0; }

    bool loadFromData(int w, int h, int depth, const GLvoid *data);

    void release();

    bool load(const CtString &fileName);

protected:
    CtTexture(bool isAtlas);
    bool loadTGA(const CtString &fileName);
    bool loadPNG(const CtString &fileName);
    bool loadPVR(const CtString &fileName);
    bool loadDDS(const CtString &fileName);

private:
    bool m_inverted;
    GLuint m_textureId;
    int m_width;
    int m_height;
    GLenum m_format;
    bool m_isAtlas;
    CtString m_error;
};

class CtAtlasTexture : public CtTexture
{
public:
    CtAtlasTexture();
    ~CtAtlasTexture();

    int tileCount() const;
    void setTileCount(int count);

    bool loadAtlas(const CtString &path);

    int indexOfKey(const CtString &key) const;

    CtRect sourceRectAt(int index) const;
    void setSourceRectAt(int index, const CtRect &rect);

    CtRect viewportRectAt(int index) const;
    void setViewportRectAt(int index, const CtRect &rect);

private:
    CtVector<CtRect> m_rects;
    CtVector<CtRect> m_drects;
    CtMap<CtString, int> m_keys;
};


class CtTextureCache
{
public:
    static CtTexture *find(const CtString &key);

    static void insert(const CtString &key, CtTexture *texture);
    static CtTexture *take(const CtString &key);
    static void remove(const CtString &key);

    static void clear();

    static CtTextureCache *instance();

private:
    CtTextureCache();
    ~CtTextureCache();

    CtMap<CtString, CtTexture *> m_map;
};

#endif
