#ifndef CTCOLOR_H
#define CTCOLOR_H

#include "ctglobal.h"

class CtColor
{
public:
    CtColor()
        : mRed(1), mGreen(1), mBlue(1), mAlpha(1) {}

    CtColor(ctreal red, ctreal green, ctreal blue, ctreal alpha = 1)
        : mRed(red), mGreen(green), mBlue(blue), mAlpha(alpha) {}

    CtColor(const CtColor &c)
        : mRed(c.mRed), mGreen(c.mGreen), mBlue(c.mBlue), mAlpha(c.mAlpha) {}

    inline ctreal red() const { return mRed; }
    inline ctreal green() const { return mGreen; }
    inline ctreal blue() const { return mBlue; }
    inline ctreal alpha() const { return mAlpha; }

    inline static CtColor fromHex(int value);

private:
    ctreal mRed;
    ctreal mGreen;
    ctreal mBlue;
    ctreal mAlpha;
};


CtColor CtColor::fromHex(int value)
{
    return CtColor(((value >> 16) & 0xFF) / 255.0f,
                   ((value >> 8) & 0xFF) / 255.0f,
                   (value & 0xFF) / 255.0f);
}

#endif
