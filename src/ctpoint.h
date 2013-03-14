#ifndef CTPOINT_H
#define CTPOINT_H

#include "ctglobal.h"


class CtPoint
{
public:
    inline CtPoint();
    inline CtPoint(ctreal x, ctreal y);
    inline CtPoint(const CtPoint &point);

    inline ctreal x() const { return mX; }
    inline ctreal y() const { return mY; }
    inline bool isValid() const { return mIsValid; }

private:
    ctreal mX;
    ctreal mY;
    bool mIsValid : 1;
};


CtPoint::CtPoint()
    : mX(0),
      mY(0),
      mIsValid(false)
{

}

CtPoint::CtPoint(ctreal x, ctreal y)
    : mX(x),
      mY(y),
      mIsValid(true)
{

}

CtPoint::CtPoint(const CtPoint &point)
    : mX(point.mX),
      mY(point.mY),
      mIsValid(point.mIsValid)
{

}


#endif
