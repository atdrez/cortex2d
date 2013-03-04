#ifndef CTFONT_H
#define CTFONT_H

#include "ctglobal.h"

class CtShaderEffect;
class CtTextureFontPrivate;

class CtTextureFont
{
public:
    CtString fileName() const;
    int fontSize() const;

    static CtTextureFont *loadBMFont(const CtString &fileName);
    static CtTextureFont *loadTTF(const CtString &fileName, int size);

private:
    CtTextureFont();
    ~CtTextureFont();

    CtTextureFontPrivate *d;

    friend class CtFontManager;
    friend class CtShaderEffect;
    friend class CtTextureFontPrivate;
};

#endif
