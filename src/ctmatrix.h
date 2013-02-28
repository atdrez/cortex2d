#ifndef CTMATRIX_H
#define CTMATRIX_H

#include "ctglobal.h"
#include "ctpoint.h"

class CtMatrix4x4;

class CtMatrix
{
public:
    CtMatrix();
    CtMatrix(const CtMatrix &matrix);

    void setIdentity();
    void setMatrix(const CtMatrix &matrix);

    void scale(ctreal sx, ctreal sy);
    void translate(ctreal tx, ctreal ty);
    void rotate(ctreal angle);
    void multiply(const CtMatrix &matrix);

    bool invert();
    CtPointReal map(ctreal x, ctreal y) const;
    void map(ctreal x, ctreal y, ctreal *ox, ctreal *oy) const;

    CtMatrix4x4 toMatrix4x4() const;

private:
    ctreal m_v11;
    ctreal m_v12;
    ctreal m_v21;
    ctreal m_v22;
    ctreal m_dx;
    ctreal m_dy;
};


class CtMatrix4x4
{
public:
    CtMatrix4x4();
    CtMatrix4x4(const CtMatrix4x4 &matrix);
    ~CtMatrix4x4();

    void setIdentity();

    ctreal *data() { return &m_data[0][0]; }

    const ctreal *constData() const {
        return const_cast<const ctreal *const>(&m_data[0][0]);
    }

    void multiply(const CtMatrix4x4 &matrix);

    bool ortho(ctreal left, ctreal right, ctreal bottom,
               ctreal top, ctreal nearZ, ctreal farZ);

private:
    ctreal m_data[4][4];
};


#endif
