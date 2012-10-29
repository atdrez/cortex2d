#ifndef CTPOINT_H
#define CTPOINT_H

#include "ctglobal.h"

class CtPoint
{
public:
    inline CtPoint();
    inline CtPoint(const CtPoint &r);
    inline CtPoint(int x, int y);

    inline int x() const { return m_x; }
    inline int y() const { return m_y; }
    inline bool isValid() const { return m_valid; }

private:
    int m_x;
    int m_y;
    bool m_valid;
};


class CtPointReal
{
public:
    inline CtPointReal();
    inline CtPointReal(const CtPointReal &r);
    inline CtPointReal(int x, int y);
    inline CtPointReal(ctreal x, ctreal y);

    inline ctreal x() const { return m_x; }
    inline ctreal y() const { return m_y; }
    inline bool isValid() const { return m_valid; }

private:
    ctreal m_x;
    ctreal m_y;
    bool m_valid;
};


CtPoint::CtPoint()
    : m_x(0),
      m_y(0),
      m_valid(false)
{

}

CtPoint::CtPoint(const CtPoint &r)
  : m_x(r.m_x),
    m_y(r.m_y),
    m_valid(r.m_valid)
{

}

CtPoint::CtPoint(int x, int y)
    : m_x(x),
      m_y(y),
      m_valid(true)
{

}


CtPointReal::CtPointReal()
    : m_x(0),
      m_y(0),
      m_valid(false)
{

}

CtPointReal::CtPointReal(const CtPointReal &r)
  : m_x(r.m_x),
    m_y(r.m_y),
    m_valid(r.m_valid)
{

}

CtPointReal::CtPointReal(int x, int y)
    : m_x(x),
      m_y(y),
      m_valid(true)
{

}

CtPointReal::CtPointReal(ctreal x, ctreal y)
    : m_x(x),
      m_y(y),
      m_valid(true)
{

}

#endif
