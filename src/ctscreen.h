#ifndef CTSCREEN_H
#define CTSCREEN_H

#include "ctglobal.h"

class CtScreenPrivate;

class CtScreen
{
public:
    static int width();
    static int height();

private:
    CtScreen();
    ~CtScreen();

    CtScreenPrivate *d;
};

#endif
