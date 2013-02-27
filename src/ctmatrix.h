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

    void scale(ctreal sx, ctreal sy);
    void translate(ctreal tx, ctreal ty);
    void rotate(ctreal angle);
    void ortho(ctreal left, ctreal right, ctreal bottom, ctreal top, ctreal nearZ, ctreal farZ);
    void multiply(const CtMatrix &matrix);

    bool invert();
    CtPointReal map(ctreal x, ctreal y) const;
    void map(ctreal x, ctreal y, ctreal *ox, ctreal *oy) const;

private:
    ctreal m[4][4];
};

#endif
