#ifndef FLMATRIX_H
#define FLMATRIX_H

#include "flglobal.h"

class FlMatrix
{
public:
    FlMatrix();
    FlMatrix(const FlMatrix &matrix);

    flreal *data() const;

    void setIdentity();
    void setMatrix(const FlMatrix &matrix);
    void setValue(int row, int col, flreal value);

    void scale(flreal sx, flreal sy, flreal sz);
    void translate(flreal tx, flreal ty, flreal tz);
    void rotate(flreal angle, flreal x, flreal y, flreal z);
    void frustum(flreal left, flreal right, flreal bottom, flreal top, flreal nearZ, flreal farZ);
    void perspective(flreal fovy, flreal aspect, flreal nearZ, flreal farZ);
    void ortho(flreal left, flreal right, flreal bottom, flreal top, flreal nearZ, flreal farZ);
    void multiply(const FlMatrix &matrix);

    bool invert();
    void map(flreal x, flreal y, flreal *ox, flreal *oy) const;

private:
    flreal m[4][4];
};

#endif
