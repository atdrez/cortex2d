#ifndef CTRECT_H
#define CTRECT_H

#include "ctlist.h"

class CtRect
{
public:
    inline CtRect();
    inline CtRect(const CtRect &r);
    inline CtRect(int x, int y, int w, int h);

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

class CtRectReal
{
public:
    inline CtRectReal();
    inline CtRectReal(const CtRectReal &r);
    inline CtRectReal(ctreal x, ctreal y, ctreal w, ctreal h);

    inline ctreal x() const { return m_x; }
    inline ctreal y() const { return m_y; }
    inline ctreal width() const { return m_w; }
    inline ctreal height() const { return m_h; }

    inline bool isValid() const { return (m_w > 0 && m_h > 0); }

private:
    ctreal m_x;
    ctreal m_y;
    ctreal m_w;
    ctreal m_h;
    bool m_valid;
};


CtRect::CtRect()
    : m_x(0),
      m_y(0),
      m_w(0),
      m_h(0),
      m_valid(false)
{

}

CtRect::CtRect(const CtRect &r)
  : m_x(r.m_x),
    m_y(r.m_y),
    m_w(r.m_w),
    m_h(r.m_h),
    m_valid(r.m_valid)
{

}

CtRect::CtRect(int x, int y, int w, int h)
    : m_x(x),
      m_y(y),
      m_w(w),
      m_h(h),
      m_valid(true)
{

}


CtRectReal::CtRectReal()
    : m_x(0),
      m_y(0),
      m_w(0),
      m_h(0),
      m_valid(false)
{

}

CtRectReal::CtRectReal(const CtRectReal &r)
  : m_x(r.m_x),
    m_y(r.m_y),
    m_w(r.m_w),
    m_h(r.m_h),
    m_valid(r.m_valid)
{

}

CtRectReal::CtRectReal(ctreal x, ctreal y, ctreal w, ctreal h)
    : m_x(x),
      m_y(y),
      m_w(w),
      m_h(h),
      m_valid(true)
{

}

#endif
