#ifndef FLRECT_H
#define FLRECT_H

#include "fllist.h"

class FlRect
{
public:
    inline FlRect();
    inline FlRect(const FlRect &r);
    inline FlRect(int x, int y, int w, int h);

    inline int x() const { return m_x; }
    inline int y() const { return m_y; }
    inline int width() const { return m_w; }
    inline int height() const { return m_h; }

    inline bool isValid() const { return (m_w > 0 && m_h > 0); }

private:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    bool m_valid;
};

class FlRectReal
{
public:
    inline FlRectReal();
    inline FlRectReal(const FlRectReal &r);
    inline FlRectReal(flreal x, flreal y, flreal w, flreal h);

    inline flreal x() const { return m_x; }
    inline flreal y() const { return m_y; }
    inline flreal width() const { return m_w; }
    inline flreal height() const { return m_h; }

    inline bool isValid() const { return (m_w > 0 && m_h > 0); }

private:
    flreal m_x;
    flreal m_y;
    flreal m_w;
    flreal m_h;
    bool m_valid;
};


FlRect::FlRect()
    : m_x(0),
      m_y(0),
      m_w(0),
      m_h(0),
      m_valid(false)
{

}

FlRect::FlRect(const FlRect &r)
  : m_x(r.m_x),
    m_y(r.m_y),
    m_w(r.m_w),
    m_h(r.m_h),
    m_valid(r.m_valid)
{

}

FlRect::FlRect(int x, int y, int w, int h)
    : m_x(x),
      m_y(y),
      m_w(w),
      m_h(h),
      m_valid(true)
{

}


FlRectReal::FlRectReal()
    : m_x(0),
      m_y(0),
      m_w(0),
      m_h(0),
      m_valid(false)
{

}

FlRectReal::FlRectReal(const FlRectReal &r)
  : m_x(r.m_x),
    m_y(r.m_y),
    m_w(r.m_w),
    m_h(r.m_h),
    m_valid(r.m_valid)
{

}

FlRectReal::FlRectReal(flreal x, flreal y, flreal w, flreal h)
    : m_x(x),
      m_y(y),
      m_w(w),
      m_h(h),
      m_valid(true)
{

}

#endif
