#include "ctfont.h"
#include "ctfont_p.h"
#include "cttexture.h"


CtTextureFontPrivate::CtTextureFontPrivate()
    : font(0)
{

}

CtTextureFontPrivate::~CtTextureFontPrivate()
{
    if (font) {
        texture_font_delete(font);
        font = 0;
    }
}

bool CtTextureFontPrivate::load(CtAtlasTexture *atlas, texture_atlas_t *atlasMap,
                                const char *fileName, int size)
{
    if (font)
        texture_font_delete(font);

    font = texture_font_new(atlasMap, fileName, size);

    if (!font)
        return false;

    static wchar_t *charmap = L"ABCDEFGHIJKLMNOPQRSTUVXYZW "
        "abcdefghijklmnopqrstuvxyzw"
        "0123456789"
        "|/:;,.<>[]{}()+-_!@#$%&*";

    texture_font_load_glyphs(font, charmap);

    return atlas->loadFromData(atlasMap->width, atlasMap->height,
                               atlasMap->depth, atlasMap->data);
}


CtTextureFont::CtTextureFont()
    : d(new CtTextureFontPrivate)
{

}

CtTextureFont::~CtTextureFont()
{
    delete d;
}

CtString CtTextureFont::fileName() const
{
    if (!d->font)
        return CtString();

    return CtString(d->font->filename);
}

int CtTextureFont::fontSize() const
{
    return !d->font ? 0 : d->font->size;
}
