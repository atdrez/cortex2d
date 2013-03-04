#include "ctfontmanager.h"
#include "ctfont.h"
#include "ctfont_p.h"
#include "cttexture.h"
#include "ctfontmanager_p.h"


CtFontManagerPrivate::CtFontManagerPrivate()
{

}

CtFontManagerPrivate::~CtFontManagerPrivate()
{

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

bool CtFontManager::registerBMFont(const CtString &key, const CtString &fileName)
{
    CtFontManagerPrivate *d = CtFontManagerPrivate::instance();

    CtTextureFont *font = d->fontHash.value(key, 0);

    if (font)
        return false;

    font = CtTextureFont::loadBMFont(fileName);

    if (!font) {
        return false;
    } else {
        d->fontHash[key] = font;
        return true;
    }
}

bool CtFontManager::registerFont(const CtString &key, const CtString &fileName, int size)
{
    CtFontManagerPrivate *d = CtFontManagerPrivate::instance();

    CtTextureFont *font = d->fontHash.value(key, 0);

    if (font)
        return false;

    font = CtTextureFont::loadTTF(fileName, size);

    if (!font) {
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
