#include "ctfontmanager.h"
#include "ctfont.h"
#include "ctfont_p.h"
#include "cttexture.h"
#include "ctfontmanager_p.h"


CtFontManagerPrivate::CtFontManagerPrivate()
{
    atlas = new CtAtlasTexture();
    // XXX
    atlasMap = texture_atlas_new(1024, 1024, 1);
}

CtFontManagerPrivate::~CtFontManagerPrivate()
{
    texture_atlas_delete(atlasMap);
    delete atlas;
}

CtFontManagerPrivate *CtFontManagerPrivate::instance()
{
    static CtFontManagerPrivate *result = 0;

    if (!result)
        result = new CtFontManagerPrivate();

    return result;
}


CtFontManager::CtFontManager()
{

}

CtFontManager::~CtFontManager()
{

}

bool CtFontManager::registerFont(const CtString &key, const CtString &fileName, int size)
{
    CtFontManagerPrivate *d = CtFontManagerPrivate::instance();

    CtTextureFont *font = d->fontHash.value(key, 0);

    if (font)
        return false;

    font = new CtTextureFont();
    bool ok = font->d->load(d->atlas, d->atlasMap, fileName.c_str(), size);

    if (!ok) {
        delete font;
        return false;
    } else {
        d->fontHash[key] = font;
        return true;
    }
}

bool CtFontManager::releaseFont(const CtString &key)
{
    CtFontManagerPrivate *d = CtFontManagerPrivate::instance();

    CtTextureFont *font = d->fontHash.value(key, 0);

    if (!font)
        return false;

    d->fontHash.remove(key);
    delete font;

    return true;
}

CtTextureFont *CtFontManager::findFont(const CtString &key)
{
    CtFontManagerPrivate *d = CtFontManagerPrivate::instance();
    return d->fontHash.value(key, 0);
}
