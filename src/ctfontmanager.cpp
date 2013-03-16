#include "ctfontmanager.h"
#include "ctfont.h"
#include "cttexture.h"


CtFontManager::CtFontManager()
{

}

CtFontManager::~CtFontManager()
{

}

CtFontManager *CtFontManager::instance()
{
    static CtFontManager *result = 0;

    if (!result)
        result = new CtFontManager();

    return result;
}

bool CtFontManager::registerBMFont(const CtString &key, const CtString &fileName)
{
    CtFontManager *d = CtFontManager::instance();

    CtFont *font = d->mFontHash.value(key, 0);

    if (font)
        return false;

    font = new CtFont();

    if (!font->loadBMFont(fileName)) {
        delete font;
        return false;
    } else {
        d->mFontHash[key] = font;
        return true;
    }
}

bool CtFontManager::registerFont(const CtString &key, const CtString &fileName, int size)
{
    CtFontManager *d = CtFontManager::instance();

    CtFont *font = d->mFontHash.value(key, 0);

    if (font)
        return false;

    font = new CtFont();

    if (!font->loadTTF(fileName, size)) {
        delete font;
        return false;
    } else {
        d->mFontHash[key] = font;
        return true;
    }
}

bool CtFontManager::releaseFont(const CtString &key)
{
    CtFontManager *d = CtFontManager::instance();

    CtFont *font = d->mFontHash.value(key, 0);

    if (!font)
        return false;

    d->mFontHash.remove(key);
    delete font;

    return true;
}

CtFont *CtFontManager::findFont(const CtString &key)
{
    CtFontManager *d = CtFontManager::instance();
    return d->mFontHash.value(key, 0);
}
