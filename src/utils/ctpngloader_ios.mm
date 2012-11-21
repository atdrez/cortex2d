#include "ctpngloader.h"
#include <string.h>
#import <UIKit/UIKit.h>


CtPNGTexture::CtPNGTexture()
    : width(0),
      height(0),
      bytesPerPixel(0),
      buffer(0),
      bufferSize(0)
{

}

CtPNGTexture::~CtPNGTexture()
{
    clear();
}

void CtPNGTexture::clear()
{
    if (buffer) {
        delete [] buffer;
        buffer = 0;
    }

    width = 0;
    height = 0;
    bytesPerPixel = 0;
    bufferSize = 0;
}

bool CtPNGTexture::loadFromFile(const char *fileName)
{
    NSString *path = [[NSString alloc] initWithUTF8String:fileName];

    NSData *texData = [[NSData alloc] initWithContentsOfFile:path];

    if (texData == nil) {
        return false;
    }

    UIImage *image = [[UIImage alloc] initWithData:texData];

    if (image == nil) {
        [texData release];
        return false;
    }

    width = CGImageGetWidth(image.CGImage);
    height = CGImageGetHeight(image.CGImage);

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

    // XXX: supports only RGBA
    bufferSize = height * width * 4;

    void *imageData = new unsigned char[bufferSize];

    CGContextRef imgcontext = CGBitmapContextCreate(imageData, width, height,
         8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);

    CGColorSpaceRelease(colorSpace);

    CGContextClearRect(imgcontext, CGRectMake(0, 0, width, height));
    CGContextDrawImage(imgcontext, CGRectMake(0, 0, width, height), image.CGImage);

    CGContextRelease(imgcontext);

    bytesPerPixel = 4;
    buffer = (unsigned char *)imageData;

    [image release];
    [texData release];

    return true;
}
