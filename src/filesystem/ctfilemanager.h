#ifndef CTFILEMANAGER_H
#define CTFILEMANAGER_H

#include "ctglobal.h"
#include "ctstring.h"

class CtFileManager
{
public:
    CtFileManager();
    ~CtFileManager();

    CtString currentDir() const;
    CtString workingDir() const;

    bool exists(const CtString &path) const;
    bool copy(const CtString &srcPath, const CtString &destPath);

    CtString concatPath(const CtString &path, const CtString &subPath) const;

    static CtFileManager *instance();
};

#endif
