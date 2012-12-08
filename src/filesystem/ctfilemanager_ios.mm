#include "ctfilemanager.h"
#import <UIKit/UIKit.h>


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
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *path = [fileManager currentDirectoryPath];
    return CtString([path UTF8String]);
}

CtString CtFileManager::workingDir() const
{
    NSString *path = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    return CtString([path UTF8String]);
}

bool CtFileManager::exists(const CtString &path) const
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *nsPath = [NSString stringWithUTF8String: path.c_str()];
    return [fileManager fileExistsAtPath: nsPath];
}

bool CtFileManager::copy(const CtString &srcPath, const CtString &destPath)
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *srcURL = [NSString stringWithUTF8String: srcPath.c_str()];
    NSString *destURL = [NSString stringWithUTF8String: destPath.c_str()];

    NSError *err = nil;
    return [fileManager copyItemAtPath:srcURL toPath:destURL error:&err];
}

CtString CtFileManager::concatPath(const CtString &path, const CtString &subPath) const
{
    if (path.empty())
        return subPath;

    if (path.at(path.length() - 1) == '/')
        return path + subPath;
    else
        return path + "/" + subPath;
}
