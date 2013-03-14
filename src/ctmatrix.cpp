#include "ctmatrix.h"
#include "ctmath.h"
#include <string.h>


CtMatrix::CtMatrix()
{
    setIdentity();
}

CtMatrix::CtMatrix(const CtMatrix &matrix)
{
    setMatrix(matrix);
}

void CtMatrix::setIdentity()
{
    mV11 = 1;
    mV12 = 0;
    mV21 = 0;
    mV22 = 1;
    mDx = 0;
    mDy = 0;
}

void CtMatrix::setMatrix(const CtMatrix &matrix)
{
    mV11 = matrix.mV11;
    mV12 = matrix.mV12;
    mV21 = matrix.mV21;
    mV22 = matrix.mV22;
    mDx = matrix.mDx;
    mDy = matrix.mDy;
}

void CtMatrix::scale(ctreal sx, ctreal sy)
{
    mV11 *= sx;
    mV12 *= sx;
    mV21 *= sy;
    mV22 *= sy;
}

void CtMatrix::translate(ctreal tx, ctreal ty)
{
    mDx += tx * mV11 + ty * mV21;
    mDy += ty * mV22 + tx * mV12;
}

void CtMatrix::rotate(ctreal angle)
{
    if (angle == 0)
        return;

    const ctreal d11 = mV11;
    const ctreal d12 = mV12;
    const ctreal d21 = mV21;
    const ctreal d22 = mV22;
    const ctreal sina = sin(-angle * M_PI / 180.0);
    const ctreal cosa = cos(-angle * M_PI / 180.0);

    mV11 = d11 * cosa + d21 * sina;
    mV12 = d12 * cosa + d22 * sina;
    mV21 = -d11 * sina + d21 * cosa;
    mV22 = -d12 * sina + d22 * cosa;
}

void CtMatrix::multiply(const CtMatrix &matrix)
{
    const ctreal d11 = mV11;
    const ctreal d12 = mV12;
    const ctreal d21 = mV21;
    const ctreal d22 = mV22;
    const ctreal dx = mDx;
    const ctreal dy = mDy;

    mV11 = d11 * matrix.mV11 + d12 * matrix.mV21;
    mV12 = d11 * matrix.mV12 + d12 * matrix.mV22;
    mV21 = d21 * matrix.mV11 + d22 * matrix.mV21;
    mV22 = d21 * matrix.mV12 + d22 * matrix.mV22;
    mDx = dx * matrix.mV11 + dy * matrix.mV21 + matrix.mDx;
    mDy = dx * matrix.mV12 + dy * matrix.mV22 + matrix.mDy;
}

CtPoint CtMatrix::map(ctreal ix, ctreal iy) const
{
    ctreal ox = ix * mV11 + iy * mV21 + mDx;
    ctreal oy = ix * mV12 + iy * mV22 + mDy;

    return CtPoint(ox, oy);
}

bool CtMatrix::invert()
{
    const ctreal det = (mV11 * mV22 - mV12 * mV21);

    if (det == 0) {
        setIdentity();
        return false;
    }

    const ctreal d11 = mV11;
    const ctreal d12 = mV12;
    const ctreal d21 = mV21;
    const ctreal d22 = mV22;
    const ctreal dx = mDx;
    const ctreal dy = mDy;
    const ctreal inv = 1.0 / det;

    mV11 = d22 * inv;
    mV12 = -d12 * inv;
    mV21 = -d21 * inv;
    mV22 = d11 * inv;
    mDx = (d21 * dy - d22 * dx) * inv;
    mDy = (d12 * dx - d11 * dy) * inv;

    return true;
}

#define CT_M4X4PTR(m, i, j) (*(m + (i * 4 + j)))

CtMatrix4x4 CtMatrix::toMatrix4x4() const
{
    CtMatrix4x4 result;

    ctreal *d = result.data();

    CT_M4X4PTR(d, 0, 0) = mV11;
    CT_M4X4PTR(d, 0, 1) = -mV21;
    CT_M4X4PTR(d, 1, 0) = -mV12;
    CT_M4X4PTR(d, 1, 1) = mV22;
    CT_M4X4PTR(d, 3, 0) = mDx;
    CT_M4X4PTR(d, 3, 1) = mDy;
    CT_M4X4PTR(d, 2, 2) = 1;
    CT_M4X4PTR(d, 3, 3) = 1;

    return result;
}



CtMatrix4x4::CtMatrix4x4()
{
    setIdentity();
}

CtMatrix4x4::CtMatrix4x4(const CtMatrix4x4 &matrix)
{
    memcpy(mData, matrix.mData, sizeof(matrix.mData));
}

CtMatrix4x4::~CtMatrix4x4()
{

}

void CtMatrix4x4::setIdentity()
{
    memset(mData, 0x0, sizeof(mData));
    mData[0][0] = 1.0;
    mData[1][1] = 1.0;
    mData[2][2] = 1.0;
    mData[3][3] = 1.0;
}

void CtMatrix4x4::multiply(const CtMatrix4x4 &matrix)
{
    CtMatrix4x4 copy = *this;

    const ctreal *const p = reinterpret_cast<const ctreal *const>(copy.mData);
    const ctreal *const m = reinterpret_cast<const ctreal *const>(matrix.mData);

#define CT_M4X4_MUL_LOOP_UNROLL(i) {                                    \
        mData[i][0] = (CT_M4X4PTR(p, i, 0) * CT_M4X4PTR(m, 0, 0)) +    \
            (CT_M4X4PTR(p, i, 1) * CT_M4X4PTR(m, 1, 0)) +               \
            (CT_M4X4PTR(p, i, 2) * CT_M4X4PTR(m, 2, 0)) +               \
            (CT_M4X4PTR(p, i, 3) * CT_M4X4PTR(m, 3, 0));                \
                                                                        \
        mData[i][1] = (CT_M4X4PTR(p, i, 0) * CT_M4X4PTR(m, 0, 1)) +    \
            (CT_M4X4PTR(p, i, 1) * CT_M4X4PTR(m, 1, 1)) +               \
            (CT_M4X4PTR(p, i, 2) * CT_M4X4PTR(m, 2, 1)) +               \
            (CT_M4X4PTR(p, i, 3) * CT_M4X4PTR(m, 3, 1));                \
                                                                        \
        mData[i][2] = (CT_M4X4PTR(p, i, 0) * CT_M4X4PTR(m, 0, 2)) +    \
            (CT_M4X4PTR(p, i, 1) * CT_M4X4PTR(m, 1, 2)) +               \
            (CT_M4X4PTR(p, i, 2) * CT_M4X4PTR(m, 2, 2)) +               \
            (CT_M4X4PTR(p, i, 3) * CT_M4X4PTR(m, 3, 2));                \
                                                                        \
        mData[i][3] = (CT_M4X4PTR(p, i, 0) * CT_M4X4PTR(m, 0, 3)) +    \
            (CT_M4X4PTR(p, i, 1) * CT_M4X4PTR(m, 1, 3)) +               \
            (CT_M4X4PTR(p, i, 2) * CT_M4X4PTR(m, 2, 3)) +               \
            (CT_M4X4PTR(p, i, 3) * CT_M4X4PTR(m, 3, 3));                \
    }

    CT_M4X4_MUL_LOOP_UNROLL(0);
    CT_M4X4_MUL_LOOP_UNROLL(1);
    CT_M4X4_MUL_LOOP_UNROLL(2);
    CT_M4X4_MUL_LOOP_UNROLL(3);

#undef CT_M4X4_MUL_LOOP_UNROLL
}

bool CtMatrix4x4::ortho(ctreal left, ctreal right, ctreal bottom,
                        ctreal top, ctreal nearZ, ctreal farZ)
{
    const ctreal deltaX = right - left;
    const ctreal deltaY = top - bottom;
    const ctreal deltaZ = farZ - nearZ;

    if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f))
        return false;

    mData[0][0] = 2.0f / deltaX;
    mData[1][1] = 2.0f / deltaY;
    mData[2][2] = -2.0f / deltaZ;
    mData[3][0] = -(right + left) / deltaX;
    mData[3][1] = -(top + bottom) / deltaY;
    mData[3][2] = -(nearZ + farZ) / deltaZ;
    mData[3][3] = 1;

    return true;
}
