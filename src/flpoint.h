#ifndef FLPOINT_H
#define FLPOINT_H

#include "flglobal.h"

class FlPoint
{
public:
    inline FlPoint();
    inline FlPoint(const FlPoint &r);
    inline FlPoint(int x, int y);

    inline int x() const { return m_x; }
    inline int y() const { return m_y; }
    inline bool isValid() const { return m_valid; }

private:
    int m_x;
    int m_y;
    bool m_valid;
};


class FlPointReal
{
public:
    inline FlPointReal();
    inline FlPointReal(const FlPointReal &r);
    inline FlPointReal(int x, int y);
    inline FlPointReal(flreal x, flreal y);

    inline flreal x() const { return m_x; }
    inline flreal y() const { return m_y; }
    inline bool isValid() const { return m_valid; }

private:
    flreal m_x;
    flreal m_y;
    bool m_valid;
};


FlPoint::FlPoint()
    : m_x(0),
      m_y(0),
      m_valid(false)
{

}

FlPoint::FlPoint(const FlPoint &r)
  : m_x(r.m_x),
    m_y(r.m_y),
    m_valid(r.m_valid)
{

}

FlPoint::FlPoint(int x, int y)
    : m_x(x),
      m_y(y),
      m_valid(true)
{

}


FlPointReal::FlPointReal()
    : m_x(0),
      m_y(0),
      m_valid(false)
{

}

FlPointReal::FlPointReal(const FlPointReal &r)
  : m_x(r.m_x),
    m_y(r.m_y),
    m_valid(r.m_valid)
{

}

FlPointReal::FlPointReal(int x, int y)
    : m_x(x),
      m_y(y),
      m_valid(true)
{

}

FlPointReal::FlPointReal(flreal x, flreal y)
    : m_x(x),
      m_y(y),
      m_valid(true)
{

}

#endif
