#ifndef CTFONT_P_H
#define CTFONT_P_H

#include "freetype-gl.h"
#include "ctmap.h"

class CtAtlasTexture;

struct CtGlyph
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

class CtTextureFontPrivate
{
public:
    CtTextureFontPrivate();
    ~CtTextureFontPrivate();

    void release();
    bool loadTTF(const CtString &path, int size);
    bool loadBMFont(const CtString &fileName);

    static CtTextureFontPrivate *dd(CtTextureFont *q) { return q->d; }

    float fontSize;
    float fontHeight;
    float ascender;
    float descender;
    CtString fileName;
    CtAtlasTexture *atlas;
    CtMap<wchar_t, CtGlyph *> glyphs;
};

#endif
