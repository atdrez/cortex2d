#ifndef FLTEXTURE_H
#define FLTEXTURE_H

#include "flglobal.h"
#include "flGL.h"
#include "flrect.h"
#include "flpoint.h"
#include "flmap.h"
#include "flvector.h"

class FlTexture
{
public:
    FlTexture();
    virtual ~FlTexture();

    GLuint id() const { return m_textureId; }
    GLenum format() const { return m_format; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    bool isAtlas() const { return m_isAtlas; }
    bool isInverted() const { return m_inverted; }

    FlString error() const { return m_error; }
    bool isValid() const { return m_textureId != 0; }

    void release();
    bool loadTGA(const FlString &fileName);
    bool loadPVR(const FlString &fileName);
    bool loadDDS(const FlString &fileName);

protected:
    FlTexture(bool isAtlas);

private:
    bool m_inverted;
    GLuint m_textureId;
    int m_width;
    int m_height;
    GLenum m_format;
    bool m_isAtlas;
    FlString m_error;
};

class FlAtlasTexture : public FlTexture
{
public:
    FlAtlasTexture();
    ~FlAtlasTexture();

    int tileCount() const;
    void setTileCount(int count);

    bool loadAtlas(const FlString &path);

    int indexOfKey(const FlString &key) const;

    FlRect sourceRectAt(int index) const;
    void setSourceRectAt(int index, const FlRect &rect);

    FlRect viewportRectAt(int index) const;
    void setViewportRectAt(int index, const FlRect &rect);

private:
    FlVector<FlRect> m_rects;
    FlVector<FlRect> m_drects;
    FlMap<FlString, int> m_keys;
};

#endif
