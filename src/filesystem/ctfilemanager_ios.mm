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
    NSString *nsPath = [NSString stringWithUTF8String: path.data()];
    return [fileManager fileExistsAtPath: nsPath];
}

bool CtFileManager::copy(const CtString &srcPath, const CtString &destPath)
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *srcURL = [NSString stringWithUTF8String: srcPath.data()];
    NSString *destURL = [NSString stringWithUTF8String: destPath.data()];

    NSError *err = nil;
    return [fileManager copyItemAtPath:srcURL toPath:destURL error:&err];
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
