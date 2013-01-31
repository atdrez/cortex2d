#ifndef CTMATRIX_H
#define CTMATRIX_H

#include "ctglobal.h"
#include "ctpoint.h"

class CtMatrix
{
public:
    CtMatrix();
    CtMatrix(const CtMatrix &matrix);

    ctreal *data() const;

    void setIdentity();
    void setMatrix(const CtMatrix &matrix);
    void setValue(int row, int col, ctreal value);

    void scale(ctreal sx, ctreal sy, ctreal sz);
    void translate(ctreal tx, ctreal ty, ctreal tz);
    void rotate(ctreal angle, ctreal x, ctreal y, ctreal z);
    void frustum(ctreal left, ctreal right, ctreal bottom, ctreal top, ctreal nearZ, ctreal farZ);
    void perspective(ctreal fovy, ctreal aspect, ctreal nearZ, ctreal farZ);
    void ortho(ctreal left, ctreal right, ctreal bottom, ctreal top, ctreal nearZ, ctreal farZ);
    void multiply(const CtMatrix &matrix);

    bool invert();
    CtPointReal map(ctreal x, ctreal y) const;
    void map(ctreal x, ctreal y, ctreal *ox, ctreal *oy) const;

private:
    ctreal m[4][4];
};

#endif
