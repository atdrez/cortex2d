#ifndef CTFONT_H
#define CTFONT_H

#include "ctglobal.h"
#include "ctmap.h"
#include "ctpool.h"

struct CtFontGlyph;
class CtAtlasTexture;
class CtShaderEffect;

struct CtFontGlyph
{
    float s0;
    float t0;
    float s1;
    float t1;
    size_t width;
    size_t height;
    int xOffset;
    int yOffset;
    float xAdvance;
    float yAdvance;
    wchar_t charcode;
    CtMap<wchar_t, float> kernings;
};

class CtFont
{
public:
    CtFont();
    ~CtFont();

    int fontSize() const { return mFontSize; }
    CtString fileName() const { return mFileName; }

    bool loadBMFont(const CtString &fileName);
    bool loadTTF(const CtString &path, int size);

private:
    void release();

    float mFontSize;
    float mFontHeight;
    float mAscender;
    float mDescender;
    CtString mFileName;
    CtAtlasTexture *mAtlas;
    CtMap<wchar_t, CtFontGlyph *> mGlyphs;

    friend class CtFontManager;
    friend class CtSceneText;
    friend class CtShaderEffect;
    friend class CtSceneTextPrivate;
};

typedef CtPool<CtFont *> CtFontPool;

#endif
