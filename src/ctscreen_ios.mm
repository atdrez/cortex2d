#include "ctscreen.h"
#import <UIKit/UIKit.h>


struct CtScreenPrivate
{

};


CtScreen::CtScreen()
    : d(new CtScreenPrivate)
{

}

CtScreen::~CtScreen()
{
    delete d;
}

int CtScreen::width()
{
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    return screenBounds.size.width * [[UIScreen mainScreen] scale];
}

int CtScreen::height()
{
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    return screenBounds.size.height * [[UIScreen mainScreen] scale];
}
