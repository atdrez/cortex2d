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

    CtPoint map(ctreal x, ctreal y) const;
    CtPoint map(const CtPoint &point) const { return map(point.x(), point.y()); }

    CtMatrix4x4 toMatrix4x4() const;

private:
    ctreal mV11;
    ctreal mV12;
    ctreal mV21;
    ctreal mV22;
    ctreal mDx;
    ctreal mDy;
};


class CtMatrix4x4
{
public:
    CtMatrix4x4();
    CtMatrix4x4(const CtMatrix4x4 &matrix);
    ~CtMatrix4x4();

    void setIdentity();

    ctreal *data() { return &mData[0][0]; }

    const ctreal *constData() const {
        return const_cast<const ctreal *const>(&mData[0][0]);
    }

    void multiply(const CtMatrix4x4 &matrix);

    bool ortho(ctreal left, ctreal right, ctreal bottom,
               ctreal top, ctreal nearZ, ctreal farZ);

private:
    ctreal mData[4][4];
};


#endif
