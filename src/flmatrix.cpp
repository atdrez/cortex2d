#include "flmatrix.h"
#include "flmath.h"
#include <string.h>


static inline flreal matrix_determinant_2x2(const flreal m[4][4],
                                            int c0, int c1, int r0, int r1)
{
    return m[c0][r0] * m[c1][r1] - m[c0][r1] * m[c1][r0];
}

static inline flreal matrix_determinant_3x3(const flreal m[4][4],
                                            int c0, int c1, int c2,
                                            int r0, int r1, int r2)
{
    return m[c0][r0] * matrix_determinant_2x2(m, c1, c2, r1, r2)
        - m[c1][r0] * matrix_determinant_2x2(m, c0, c2, r1, r2)
        + m[c2][r0] * matrix_determinant_2x2(m, c0, c1, r1, r2);
}

static inline flreal matrix_determinant_4x4(const flreal m[4][4])
{
    return m[0][0] * matrix_determinant_3x3(m, 1, 2, 3, 1, 2, 3)
        - m[1][0] * matrix_determinant_3x3(m, 0, 2, 3, 1, 2, 3)
        + m[2][0] * matrix_determinant_3x3(m, 0, 1, 3, 1, 2, 3)
        - m[3][0] * matrix_determinant_3x3(m, 0, 1, 2, 1, 2, 3);
}


FlMatrix::FlMatrix()
{
    setIdentity();
}

FlMatrix::FlMatrix(const FlMatrix &matrix)
{
    setMatrix(matrix);
}

flreal *FlMatrix::data() const
{
    return (flreal *)(&m);
}

void FlMatrix::setIdentity()
{
    memset(m, 0x0, sizeof(m));
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
}

void FlMatrix::setMatrix(const FlMatrix &matrix)
{
    memcpy(m, matrix.m, sizeof(matrix.m));
}

void FlMatrix::setValue(int row, int col, flreal value)
{
    if (row >= 0 && row < 4 && col >= 0 && col < 4)
        m[col][row] = value;
}

void FlMatrix::scale(flreal sx, flreal sy, flreal sz)
{
    m[0][0] *= sx;
    m[0][1] *= sx;
    m[0][2] *= sx;
    m[0][3] *= sx;

    m[1][0] *= sy;
    m[1][1] *= sy;
    m[1][2] *= sy;
    m[1][3] *= sy;

    m[2][0] *= sz;
    m[2][1] *= sz;
    m[2][2] *= sz;
    m[2][3] *= sz;
}

void FlMatrix::translate(flreal tx, flreal ty, flreal tz)
{
    m[3][0] += (m[0][0] * tx + m[1][0] * ty + m[2][0] * tz);
    m[3][1] += (m[0][1] * tx + m[1][1] * ty + m[2][1] * tz);
    m[3][2] += (m[0][2] * tx + m[1][2] * ty + m[2][2] * tz);
    m[3][3] += (m[0][3] * tx + m[1][3] * ty + m[2][3] * tz);
}

void FlMatrix::rotate(flreal angle, flreal x, flreal y, flreal z)
{
    flreal sinAngle, cosAngle;
    flreal mag = sqrtf(x * x + y * y + z * z);

    sinAngle = sinf ( angle * FL_PI / 180.0f );
    cosAngle = cosf ( angle * FL_PI / 180.0f );

    if (mag > 0.0f) {
      flreal xx, yy, zz, xy, yz, zx, xs, ys, zs;
      flreal oneMinusCos;
      FlMatrix rotMat;

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

void FlMatrix::frustum(flreal left, flreal right, flreal bottom, flreal top, flreal nearZ, flreal farZ)
{
    flreal       deltaX = right - left;
    flreal       deltaY = top - bottom;
    flreal       deltaZ = farZ - nearZ;
    FlMatrix    frust;

    if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
        (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
        return;

    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;

    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

    frust.multiply(*this);
    setMatrix(frust);
}

void FlMatrix::perspective(flreal fovy, flreal aspect, flreal nearZ, flreal farZ)
{
   flreal frustumW, frustumH;

   frustumH = tanf( fovy / 360.0f * FL_PI) * nearZ;
   frustumW = frustumH * aspect;

   frustum(-frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

void FlMatrix::ortho(flreal left, flreal right, flreal bottom, flreal top, flreal nearZ, flreal farZ)
{
    flreal       deltaX = right - left;
    flreal       deltaY = top - bottom;
    flreal       deltaZ = farZ - nearZ;
    FlMatrix    ortho;

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

void FlMatrix::multiply(const FlMatrix &matrix)
{
    FlMatrix tmp;

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

void FlMatrix::map(flreal ix, flreal iy, flreal *ox, flreal *oy) const
{
    flreal x = ix * m[0][0] + iy * m[1][0] + m[3][0];
    flreal y = ix * m[0][1] + iy * m[1][1] + m[3][1];
    flreal w = ix * m[0][3] + iy * m[1][3] + m[3][3];

    if (w == 1.0f) {
        *ox = x;
        *oy = y;
    } else {
        *ox = x / w;
        *oy = y / w;
    }
}

bool FlMatrix::invert()
{
    flreal det = matrix_determinant_4x4(m);

    if (det == 0.0f)
        return false;

    FlMatrix tmp;
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
