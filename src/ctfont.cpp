#include "ctfont.h"
#include "ctfile.h"
#include "ctbuffer.h"
#include "cttexture.h"
#include "freetype-gl.h"
#include "bmfontparser.h"

static wchar_t *charmap = L"ABCDEFGHIJKLMNOPQRSTUVXYZW "
    "abcdefghijklmnopqrstuvxyzw"
    "0123456789"
    "|/:;,.<>[]{}()+-_!@#$%&*";

static texture_atlas_t *ct_sharedTTFTextureMap()
{
    static texture_atlas_t *result = 0;

    if (!result)
        result = texture_atlas_new(1024, 1024, 1);

    return result;
}

CtFont::CtFont()
    : mFontSize(0),
      mFontHeight(0),
      mAscender(0),
      mDescender(0),
      mAtlas(0)
{

}

CtFont::~CtFont()
{
    release();
}

void CtFont::release()
{
    CtMap<wchar_t, CtFontGlyph *>::iterator it;

    for (it = mGlyphs.begin(); it != mGlyphs.end(); it++)
        delete it->second;

    mGlyphs.clear();

    if (mAtlas) {
        delete mAtlas;
        mAtlas = 0;
    }
}

bool CtFont::loadTTF(const CtString &path, int size)
{
    release();

    mFileName = path;
    mFontSize = size;

    static texture_atlas_t *textureMap = ct_sharedTTFTextureMap();

    CT_ASSERT(textureMap != 0);

    texture_font_t *font = texture_font_new(textureMap, path.data(), size);

    if (!font)
        return false;

    mFontHeight = font->height;
    mAscender = font->ascender;
    mDescender = font->descender;

    texture_atlas_clear(textureMap);
    texture_font_load_glyphs(font, charmap);

    mAtlas = new CtAtlasTexture();
    bool ok = mAtlas->loadFromData(textureMap->width, textureMap->height,
                                  textureMap->depth, textureMap->data);

    if (!ok) {
        release();
        texture_font_delete(font);
        return false;
    }

    for (wchar_t *c = charmap; *c; c++) {
        const wchar_t ch = *c;
        texture_glyph_t *glyph = texture_font_get_glyph(font, ch);

        if (!glyph)
            continue;

        CtFontGlyph *fontGlyph = new CtFontGlyph();
        fontGlyph->s0 = glyph->s0;
        fontGlyph->t0 = glyph->t0;
        fontGlyph->s1 = glyph->s1;
        fontGlyph->t1 = glyph->t1;
        fontGlyph->width = glyph->width;
        fontGlyph->height = glyph->height;
        fontGlyph->xOffset = glyph->offset_x;
        fontGlyph->yOffset = glyph->offset_y;
        fontGlyph->xAdvance = glyph->advance_x;
        fontGlyph->yAdvance = glyph->advance_y;
        fontGlyph->charcode = glyph->charcode;

        mGlyphs.insert(ch, fontGlyph);

        for (size_t i = 0; i < vector_size(glyph->kerning); i++) {
            kerning_t *kerning = (kerning_t *)vector_get(glyph->kerning, i);
            fontGlyph->kernings.insert(kerning->charcode, kerning->kerning);
        }
    }

    texture_font_delete(font);

    return true;
}

bool CtFont::loadBMFont(const CtString &path)
{
    release();

    CtFile fp(path);

    if (!fp.open(CtFile::ReadOnly))
        return false;

    mFileName = path;

    int idx = mFileName.lastIndexOf('.');

    if (idx < 0)
        return false;

    // TODO: handle different extensions
    CtString texturePath = mFileName.substr(0, idx) + CtString(".tga");

    // read file content
    int bufferSize = (int)fp.size();
    CtBuffer buffer(bufferSize);

    if (!fp.readBuffer(&buffer))
        return false;

    char *str = (char *)buffer.data();
    str[bufferSize - 1] = 0;

    // parse file content
    BMFont *font = bmFontParse((const char *)str, 0);

    if (!font)
        return false;

    float scaleW = font->common.scaleW;
    float scaleH = font->common.scaleH;

    mAscender = font->common.base;
    mFontHeight = font->common.lineHeight;

    mAtlas = new CtAtlasTexture();

    if (!mAtlas->load(texturePath))
        return false;


    BMFontChar *glyph = font->chars;

    while (glyph) {
        int charcode = glyph->id;
        float s0 = float(glyph->x) / scaleW;
        float t0 = float(glyph->y) / scaleH;
        float s1 = s0 + float(glyph->width) / scaleW;
        float t1 = t0 + float(glyph->height) / scaleH;

        CtFontGlyph *fontGlyph = new CtFontGlyph();
        fontGlyph->s0 = s0;
        fontGlyph->t0 = t0;
        fontGlyph->s1 = s1;
        fontGlyph->t1 = t1;
        fontGlyph->width = glyph->width;
        fontGlyph->height = glyph->height;
        fontGlyph->xOffset = glyph->xoffset;
        fontGlyph->yOffset = glyph->yoffset;
        fontGlyph->xAdvance = glyph->xadvance;
        fontGlyph->yAdvance = 0;
        fontGlyph->charcode = charcode;

        mGlyphs.insert((wchar_t)charcode, fontGlyph);

        glyph = glyph->next;
    }

    BMFontKerning *kerning = font->kernings;

    while (kerning) {
        CtFontGlyph *glyph = mGlyphs.value(kerning->first, 0);

        if (glyph)
            glyph->kernings.insert(kerning->second, kerning->amount);

        kerning = kerning->next;
    }

    bmFontDelete(font);

    return true;
}
