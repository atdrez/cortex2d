#ifndef CTFONTMANAGER_P_H
#define CTFONTMANAGER_P_H

#include "ctlist.h"
#include "freetype-gl.h"

class CtTextureFont;
class CtAtlasTexture;

struct CtFontManagerPrivate
{
    CtFontManagerPrivate();
    ~CtFontManagerPrivate();

    static CtFontManagerPrivate *instance();

    CtMap<CtString, CtTextureFont *> fontHash;
};

#endif
