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
    m_v11 = 1;
    m_v12 = 0;
    m_v21 = 0;
    m_v22 = 1;
    m_dx = 0;
    m_dy = 0;
}

void CtMatrix::setMatrix(const CtMatrix &matrix)
{
    m_v11 = matrix.m_v11;
    m_v12 = matrix.m_v12;
    m_v21 = matrix.m_v21;
    m_v22 = matrix.m_v22;
    m_dx = matrix.m_dx;
    m_dy = matrix.m_dy;
}

void CtMatrix::scale(ctreal sx, ctreal sy)
{
    m_v11 *= sx;
    m_v12 *= sx;
    m_v21 *= sy;
    m_v22 *= sy;
}

void CtMatrix::translate(ctreal tx, ctreal ty)
{
    m_dx += tx * m_v11 + ty * m_v21;
    m_dy += ty * m_v22 + tx * m_v12;
}

void CtMatrix::rotate(ctreal angle)
{
    if (angle == 0)
        return;

    const ctreal d11 = m_v11;
    const ctreal d12 = m_v12;
    const ctreal d21 = m_v21;
    const ctreal d22 = m_v22;
    const ctreal sina = sin(-angle * M_PI / 180.0);
    const ctreal cosa = cos(-angle * M_PI / 180.0);

    m_v11 = d11 * cosa + d21 * sina;
    m_v12 = d12 * cosa + d22 * sina;
    m_v21 = -d11 * sina + d21 * cosa;
    m_v22 = -d12 * sina + d22 * cosa;
}

void CtMatrix::multiply(const CtMatrix &matrix)
{
    const ctreal d11 = m_v11;
    const ctreal d12 = m_v12;
    const ctreal d21 = m_v21;
    const ctreal d22 = m_v22;
    const ctreal dx = m_dx;
    const ctreal dy = m_dy;

    m_v11 = d11 * matrix.m_v11 + d12 * matrix.m_v21;
    m_v12 = d11 * matrix.m_v12 + d12 * matrix.m_v22;
    m_v21 = d21 * matrix.m_v11 + d22 * matrix.m_v21;
    m_v22 = d21 * matrix.m_v12 + d22 * matrix.m_v22;
    m_dx = dx * matrix.m_v11 + dy * matrix.m_v21 + matrix.m_dx;
    m_dy = dx * matrix.m_v12 + dy * matrix.m_v22 + matrix.m_dy;
}

CtPointReal CtMatrix::map(ctreal x, ctreal y) const
{
    ctreal ox, oy;
    map(x, y, &ox, &oy);
    return CtPointReal(ox, oy);
}

void CtMatrix::map(ctreal ix, ctreal iy, ctreal *ox, ctreal *oy) const
{
    *ox = ix * m_v11 + iy * m_v21 + m_dx;
    *oy = ix * m_v12 + iy * m_v22 + m_dy;
}

bool CtMatrix::invert()
{
    const ctreal det = (m_v11 * m_v22 - m_v12 * m_v21);

    if (det == 0) {
        setIdentity();
        return false;
    }

    const ctreal d11 = m_v11;
    const ctreal d12 = m_v12;
    const ctreal d21 = m_v21;
    const ctreal d22 = m_v22;
    const ctreal dx = m_dx;
    const ctreal dy = m_dy;
    const ctreal inv = 1.0 / det;

    m_v11 = d22 * inv;
    m_v12 = -d12 * inv;
    m_v21 = -d21 * inv;
    m_v22 = d11 * inv;
    m_dx = (d21 * dy - d22 * dx) * inv;
    m_dy = (d12 * dx - d11 * dy) * inv;

    return true;
}

#define CT_M4X4PTR(m, i, j) (*(m + (i * 4 + j)))

CtMatrix4x4 CtMatrix::toMatrix4x4() const
{
    CtMatrix4x4 result;

    ctreal *d = result.data();

    CT_M4X4PTR(d, 0, 0) = m_v11;
    CT_M4X4PTR(d, 0, 1) = -m_v21;
    CT_M4X4PTR(d, 1, 0) = -m_v12;
    CT_M4X4PTR(d, 1, 1) = m_v22;
    CT_M4X4PTR(d, 3, 0) = m_dx;
    CT_M4X4PTR(d, 3, 1) = m_dy;
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
    memcpy(m_data, matrix.m_data, sizeof(matrix.m_data));
}

CtMatrix4x4::~CtMatrix4x4()
{

}

void CtMatrix4x4::setIdentity()
{
    memset(m_data, 0x0, sizeof(m_data));
    m_data[0][0] = 1.0;
    m_data[1][1] = 1.0;
    m_data[2][2] = 1.0;
    m_data[3][3] = 1.0;
}

void CtMatrix4x4::multiply(const CtMatrix4x4 &matrix)
{
    CtMatrix4x4 copy = *this;

    const ctreal *const p = reinterpret_cast<const ctreal *const>(copy.m_data);
    const ctreal *const m = reinterpret_cast<const ctreal *const>(matrix.m_data);

#define CT_M4X4_MUL_LOOP_UNROLL(i) {                                    \
        m_data[i][0] = (CT_M4X4PTR(p, i, 0) * CT_M4X4PTR(m, 0, 0)) +    \
            (CT_M4X4PTR(p, i, 1) * CT_M4X4PTR(m, 1, 0)) +               \
            (CT_M4X4PTR(p, i, 2) * CT_M4X4PTR(m, 2, 0)) +               \
            (CT_M4X4PTR(p, i, 3) * CT_M4X4PTR(m, 3, 0));                \
                                                                        \
        m_data[i][1] = (CT_M4X4PTR(p, i, 0) * CT_M4X4PTR(m, 0, 1)) +    \
            (CT_M4X4PTR(p, i, 1) * CT_M4X4PTR(m, 1, 1)) +               \
            (CT_M4X4PTR(p, i, 2) * CT_M4X4PTR(m, 2, 1)) +               \
            (CT_M4X4PTR(p, i, 3) * CT_M4X4PTR(m, 3, 1));                \
                                                                        \
        m_data[i][2] = (CT_M4X4PTR(p, i, 0) * CT_M4X4PTR(m, 0, 2)) +    \
            (CT_M4X4PTR(p, i, 1) * CT_M4X4PTR(m, 1, 2)) +               \
            (CT_M4X4PTR(p, i, 2) * CT_M4X4PTR(m, 2, 2)) +               \
            (CT_M4X4PTR(p, i, 3) * CT_M4X4PTR(m, 3, 2));                \
                                                                        \
        m_data[i][3] = (CT_M4X4PTR(p, i, 0) * CT_M4X4PTR(m, 0, 3)) +    \
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

    m_data[0][0] = 2.0f / deltaX;
    m_data[1][1] = 2.0f / deltaY;
    m_data[2][2] = -2.0f / deltaZ;
    m_data[3][0] = -(right + left) / deltaX;
    m_data[3][1] = -(top + bottom) / deltaY;
    m_data[3][2] = -(nearZ + farZ) / deltaZ;
    m_data[3][3] = 1;

    return true;
}
