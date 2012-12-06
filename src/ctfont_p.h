#ifndef CTFONT_P_H
#define CTFONT_P_H

#include "freetype-gl.h"

class CtAtlasTexture;

class CtTextureFontPrivate
{
public:
    CtTextureFontPrivate();
    ~CtTextureFontPrivate();

    bool load(CtAtlasTexture *atlas, texture_atlas_t *atlasMap,
              const char *fileName, int size);

    static CtTextureFontPrivate *dd(CtTextureFont *q) { return q->d; }

    texture_font_t *font;
};

#endif
