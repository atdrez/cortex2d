#ifndef CTCOLOR_H
#define CTCOLOR_H

class CtColor
{
public:
    CtColor()
        : m_r(1), m_g(1), m_b(1), m_a(1) {}

    CtColor(ctreal r, ctreal g, ctreal b, ctreal a = 1)
        : m_r(r), m_g(g), m_b(b), m_a(a) {}

    CtColor(const CtColor &c)
        : m_r(c.m_r), m_g(c.m_g), m_b(c.m_b), m_a(c.m_a) {}

    inline ctreal r() const { return m_r; }
    inline ctreal g() const { return m_g; }
    inline ctreal b() const { return m_b; }
    inline ctreal a() const { return m_a; }

private:
    ctreal m_r;
    ctreal m_g;
    ctreal m_b;
    ctreal m_a;
};

#endif