#include "ctmatrix.h"
#include "ctmath.h"
#include <string.h>


static inline ctreal matrix_determinant_2x2(const ctreal m[4][4],
                                            int c0, int c1, int r0, int r1)
{
    return m[c0][r0] * m[c1][r1] - m[c0][r1] * m[c1][r0];
}

static inline ctreal matrix_determinant_3x3(const ctreal m[4][4],
                                            int c0, int c1, int c2,
                                            int r0, int r1, int r2)
{
    return m[c0][r0] * matrix_determinant_2x2(m, c1, c2, r1, r2)
        - m[c1][r0] * matrix_determinant_2x2(m, c0, c2, r1, r2)
        + m[c2][r0] * matrix_determinant_2x2(m, c0, c1, r1, r2);
}

static inline ctreal matrix_determinant_4x4(const ctreal m[4][4])
{
    return m[0][0] * matrix_determinant_3x3(m, 1, 2, 3, 1, 2, 3)
        - m[1][0] * matrix_determinant_3x3(m, 0, 2, 3, 1, 2, 3)
        + m[2][0] * matrix_determinant_3x3(m, 0, 1, 3, 1, 2, 3)
        - m[3][0] * matrix_determinant_3x3(m, 0, 1, 2, 1, 2, 3);
}


CtMatrix::CtMatrix()
{
    setIdentity();
}

CtMatrix::CtMatrix(const CtMatrix &matrix)
{
    setMatrix(matrix);
}

ctreal *CtMatrix::data() const
{
    return (ctreal *)(&m);
}

void CtMatrix::setIdentity()
{
    memset(m, 0x0, sizeof(m));
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
}

void CtMatrix::setMatrix(const CtMatrix &matrix)
{
    memcpy(m, matrix.m, sizeof(matrix.m));
}

void CtMatrix::scale(ctreal sx, ctreal sy)
{
    m[0][0] *= sx;
    m[0][1] *= sx;
    m[0][2] *= sx;
    m[0][3] *= sx;

    m[1][0] *= sy;
    m[1][1] *= sy;
    m[1][2] *= sy;
    m[1][3] *= sy;
}

void CtMatrix::translate(ctreal tx, ctreal ty)
{
    m[3][0] += (m[0][0] * tx + m[1][0] * ty);
    m[3][1] += (m[0][1] * tx + m[1][1] * ty);
    m[3][2] += (m[0][2] * tx + m[1][2] * ty);
    m[3][3] += (m[0][3] * tx + m[1][3] * ty);
}

void CtMatrix::rotate(ctreal angle)
{
    ctreal x = 0;
    ctreal y = 0;
    ctreal z = 1;

    ctreal sinAngle, cosAngle;
    ctreal mag = sqrtf(x * x + y * y + z * z);

    sinAngle = sinf ( angle * CT_PI / 180.0f );
    cosAngle = cosf ( angle * CT_PI / 180.0f );

    if (mag > 0.0f) {
      ctreal xx, yy, zz, xy, yz, zx, xs, ys, zs;
      ctreal oneMinusCos;
      CtMatrix rotMat;

      x /= mag;
      y /= mag;
      z /= mag;

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;

      rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
      rotMat.m[0][1] = (oneMinusCos * xy) - zs;
      rotMat.m[0][2] = (oneMinusCos * zx) + ys;
      rotMat.m[0][3] = 0.0F;

      rotMat.m[1][0] = (oneMinusCos * xy) + zs;
      rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
      rotMat.m[1][2] = (oneMinusCos * yz) - xs;
      rotMat.m[1][3] = 0.0F;

      rotMat.m[2][0] = (oneMinusCos * zx) - ys;
      rotMat.m[2][1] = (oneMinusCos * yz) + xs;
      rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
      rotMat.m[2][3] = 0.0F;

      rotMat.m[3][0] = 0.0F;
      rotMat.m[3][1] = 0.0F;
      rotMat.m[3][2] = 0.0F;
      rotMat.m[3][3] = 1.0F;

      rotMat.multiply(*this);
      setMatrix(rotMat);
   }
}

void CtMatrix::ortho(ctreal left, ctreal right, ctreal bottom, ctreal top, ctreal nearZ, ctreal farZ)
{
    ctreal       deltaX = right - left;
    ctreal       deltaY = top - bottom;
    ctreal       deltaZ = farZ - nearZ;
    CtMatrix    ortho;

    if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
        return;

    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

    ortho.multiply(*this);
    setMatrix(ortho);
}

void CtMatrix::multiply(const CtMatrix &matrix)
{
    CtMatrix tmp;

    for (int i = 0; i < 4; i++) {
        tmp.m[i][0] = (m[i][0] * matrix.m[0][0]) +
            (m[i][1] * matrix.m[1][0]) +
            (m[i][2] * matrix.m[2][0]) +
            (m[i][3] * matrix.m[3][0]) ;

        tmp.m[i][1] = (m[i][0] * matrix.m[0][1]) +
            (m[i][1] * matrix.m[1][1]) +
            (m[i][2] * matrix.m[2][1]) +
            (m[i][3] * matrix.m[3][1]) ;

        tmp.m[i][2] = (m[i][0] * matrix.m[0][2]) +
            (m[i][1] * matrix.m[1][2]) +
            (m[i][2] * matrix.m[2][2]) +
            (m[i][3] * matrix.m[3][2]) ;

        tmp.m[i][3] = (m[i][0] * matrix.m[0][3]) +
            (m[i][1] * matrix.m[1][3]) +
            (m[i][2] * matrix.m[2][3]) +
            (m[i][3] * matrix.m[3][3]) ;
    }

    setMatrix(tmp);
}

CtPointReal CtMatrix::map(ctreal x, ctreal y) const
{
    ctreal ox, oy;
    map(x, y, &ox, &oy);
    return CtPointReal(ox, oy);
}

void CtMatrix::map(ctreal ix, ctreal iy, ctreal *ox, ctreal *oy) const
{
    ctreal x = ix * m[0][0] + iy * m[1][0] + m[3][0];
    ctreal y = ix * m[0][1] + iy * m[1][1] + m[3][1];
    ctreal w = ix * m[0][3] + iy * m[1][3] + m[3][3];

    if (w == 1.0f) {
        *ox = x;
        *oy = y;
    } else {
        *ox = x / w;
        *oy = y / w;
    }
}

bool CtMatrix::invert()
{
    ctreal det = matrix_determinant_4x4(m);

    if (det == 0.0f)
        return false;

    CtMatrix tmp;
    det = 1.0f / det;

    tmp.m[0][0] =  matrix_determinant_3x3(m, 1, 2, 3, 1, 2, 3) * det;
    tmp.m[0][1] = -matrix_determinant_3x3(m, 0, 2, 3, 1, 2, 3) * det;
    tmp.m[0][2] =  matrix_determinant_3x3(m, 0, 1, 3, 1, 2, 3) * det;
    tmp.m[0][3] = -matrix_determinant_3x3(m, 0, 1, 2, 1, 2, 3) * det;
    tmp.m[1][0] = -matrix_determinant_3x3(m, 1, 2, 3, 0, 2, 3) * det;
    tmp.m[1][1] =  matrix_determinant_3x3(m, 0, 2, 3, 0, 2, 3) * det;
    tmp.m[1][2] = -matrix_determinant_3x3(m, 0, 1, 3, 0, 2, 3) * det;
    tmp.m[1][3] =  matrix_determinant_3x3(m, 0, 1, 2, 0, 2, 3) * det;
    tmp.m[2][0] =  matrix_determinant_3x3(m, 1, 2, 3, 0, 1, 3) * det;
    tmp.m[2][1] = -matrix_determinant_3x3(m, 0, 2, 3, 0, 1, 3) * det;
    tmp.m[2][2] =  matrix_determinant_3x3(m, 0, 1, 3, 0, 1, 3) * det;
    tmp.m[2][3] = -matrix_determinant_3x3(m, 0, 1, 2, 0, 1, 3) * det;
    tmp.m[3][0] = -matrix_determinant_3x3(m, 1, 2, 3, 0, 1, 2) * det;
    tmp.m[3][1] =  matrix_determinant_3x3(m, 0, 2, 3, 0, 1, 2) * det;
    tmp.m[3][2] = -matrix_determinant_3x3(m, 0, 1, 3, 0, 1, 2) * det;
    tmp.m[3][3] =  matrix_determinant_3x3(m, 0, 1, 2, 0, 1, 2) * det;

    setMatrix(tmp);
    return true;
}
