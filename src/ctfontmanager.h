#ifndef CTFONTMANAGER_H
#define CTFONTMANAGER_H

#include "ctglobal.h"
#include "ctmap.h"

class CtFont;
class CtFontManagerPrivate;

class CtFontManager
{
public:
    static bool registerBMFont(const CtString &key, const CtString &fileName);

    static bool registerFont(const CtString &key, const CtString &fileName, int size);
    static bool releaseFont(const CtString &key);

    static CtFont *findFont(const CtString &key);

private:
    CtFontManager();
    ~CtFontManager();

    static CtFontManager *instance();

    CtMap<CtString, CtFont *> mFontHash;
};

#endif
