#include "ctfilemanager.h"
#include "ctfile.h"
#include "ctapplication.h"
#include <fstream>


CtFileManager::CtFileManager()
{

}

CtFileManager::~CtFileManager()
{

}

CtFileManager *CtFileManager::instance()
{
    static CtFileManager *r = new CtFileManager();
    return r;
}

CtString CtFileManager::currentDir() const
{
    // XXX
    return CtApplication::instance()->applicationDir();
}

CtString CtFileManager::workingDir() const
{
    // XXX
    return CtApplication::instance()->applicationDir();
}

bool CtFileManager::exists(const CtString &path) const
{
    // XXX
    CtFile fp(path);
    bool result = fp.open(CtFile::ReadOnly);
    fp.close();
    return result;
}

bool CtFileManager::copy(const CtString &srcPath, const CtString &destPath)
{
    std::ifstream input(srcPath.data(), std::fstream::binary);
    std::ofstream output(destPath.data(), std::fstream::binary);
    output << input.rdbuf();
    input.close();
    output.close();
}

CtString CtFileManager::concatPath(const CtString &path, const CtString &subPath) const
{
    if (path.isEmpty())
        return subPath;

    if (path.at(path.length() - 1) == '/')
        return path + subPath;
    else
        return path + "/" + subPath;
}
