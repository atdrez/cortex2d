#ifndef CTRECT_H
#define CTRECT_H

#include "ctglobal.h"


class CtRect
{
public:
    inline CtRect();
    inline CtRect(const CtRect &rect);
    inline CtRect(ctreal x, ctreal y, ctreal w, ctreal h);

    inline ctreal x() const { return mX; }
    inline ctreal y() const { return mY; }
    inline ctreal width() const { return mWidth; }
    inline ctreal height() const { return mHeight; }

    inline bool isNull() const { return mIsNull; }
    inline bool isValid() const { return (mWidth > 0 && mHeight > 0); }

private:
    ctreal mX;
    ctreal mY;
    ctreal mWidth;
    ctreal mHeight;
    bool mIsNull : 1;
};


CtRect::CtRect()
    : mX(0),
      mY(0),
      mWidth(0),
      mHeight(0),
      mIsNull(true)
{

}

CtRect::CtRect(const CtRect &rect)
    : mX(rect.mX),
      mY(rect.mY),
      mWidth(rect.mWidth),
      mHeight(rect.mHeight),
      mIsNull(rect.mIsNull)
{

}

CtRect::CtRect(ctreal x, ctreal y, ctreal w, ctreal h)
    : mX(x),
      mY(y),
      mWidth(w),
      mHeight(h),
      mIsNull(false)
{

}

#endif
