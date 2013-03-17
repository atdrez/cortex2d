#ifndef CTTEXTURE_H
#define CTTEXTURE_H

#include "ctGL.h"
#include "ctrect.h"
#include "ctmap.h"
#include "ctpool.h"
#include "ctglobal.h"
#include "ctvector.h"


class CtTexture
{
public:
    CtTexture();
    virtual ~CtTexture();

    inline GLuint id() const { return mId; }
    inline GLenum format() const { return mFormat; }

    inline int width() const { return mWidth; }
    inline int height() const { return mHeight; }

    inline bool isAtlas() const { return mIsAtlas; }
    inline bool isInverted() const { return mIsInverted; }

    inline CtString error() const { return mError; }
    inline bool isValid() const { return mId != 0; }

    bool load(const CtString &fileName);
    bool loadFromData(int w, int h, int depth, const GLvoid *data);

    void release();

protected:
    CtTexture(bool isAtlas);
    void setError(const CtString &error);
    bool loadTGA(const CtString &fileName);
    bool loadPNG(const CtString &fileName);
    bool loadPVR(const CtString &fileName);
    bool loadDDS(const CtString &fileName);

private:
    GLuint mId;
    int mWidth;
    int mHeight;
    GLenum mFormat;
    bool mIsAtlas;
    bool mIsInverted;
    CtString mError;

    CT_PRIVATE_COPY(CtTexture);
};

class CtAtlasTexture : public CtTexture
{
public:
    CtAtlasTexture();
    ~CtAtlasTexture();

    int tileCount() const;
    int indexOfKey(const CtString &key) const;

    bool loadAtlas(const CtString &path);

    CtRect sourceRectAt(int index) const;
    CtRect viewportRectAt(int index) const;

private:
    CtMap<CtString, int> mKeys;
    CtVector<CtRect> mSourceRects;
    CtVector<CtRect> mViewportRects;

    CT_PRIVATE_COPY(CtAtlasTexture);
};


typedef CtPool<CtTexture> CtTexturePool;


#endif
