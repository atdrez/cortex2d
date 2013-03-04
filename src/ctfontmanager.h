#ifndef CTFONTMANAGER_H
#define CTFONTMANAGER_H

#include "ctglobal.h"

class CtTextureFont;
class CtFontManagerPrivate;

class CtFontManager
{
public:
    static bool registerBMFont(const CtString &key, const CtString &fileName);

    static bool registerFont(const CtString &key, const CtString &fileName, int size);
    static bool releaseFont(const CtString &key);

    static CtTextureFont *findFont(const CtString &key);

private:
    CtFontManager();
    ~CtFontManager();
};

#endif
