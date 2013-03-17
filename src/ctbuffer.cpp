#include "ctbuffer.h"
#include <string.h>

template<typename T>
static inline void ct_readType(T *value, ctubyte *d, int i)
{
    const size_t size = sizeof(T);
    ctubyte *v = reinterpret_cast<ctubyte *>(value);

    CT_ASSERT(size <= 8);

    // XXX: check endianness
    switch (size) {
    case 8:
        v[7] = d[i + 7];
    case 7:
        v[6] = d[i + 6];
    case 6:
        v[5] = d[i + 5];
    case 5:
        v[4] = d[i + 4];
    case 4:
        v[3] = d[i + 3];
    case 3:
        v[2] = d[i + 2];
    case 2:
        v[1] = d[i + 1];
    case 1:
        v[0] = d[i];
        break;
    default:
        *value = 0;
        break;
    }
}


CtBuffer::CtBuffer(int size)
    : m_size(size),
      m_data(size > 0 ? new ctubyte[size] : 0)
{

}

CtBuffer::~CtBuffer()
{
    if (m_data)
        delete [] m_data;
}

ctbyte CtBuffer::readByte(int offset) const
{
    if (offset < 0 || offset >= m_size)
        return 0;

    ctbyte result;
    ct_readType(&result, m_data, offset);
    return result;
}

ctubyte CtBuffer::readUByte(int offset) const
{
    if (offset < 0 || offset >= m_size)
        return 0;

    ctubyte result;
    ct_readType(&result, m_data, offset);
    return result;
}

ctint16 CtBuffer::readInt16(int offset) const
{
    if (offset < 0 || offset >= m_size)
        return 0;

    ctint16 result;
    ct_readType(&result, m_data, offset);
    return result;
}

ctuint16 CtBuffer::readUInt16(int offset) const
{
    if (offset < 0 || offset >= m_size)
        return 0;

    ctuint16 result;
    ct_readType(&result, m_data, offset);
    return result;
}

ctint32 CtBuffer::readInt32(int offset) const
{
    ctint32 result;
    ct_readType(&result, m_data, offset);
    return result;
}

ctuint32 CtBuffer::readUInt32(int offset) const
{
    ctuint32 result;
    ct_readType(&result, m_data, offset);
    return result;
}

ctint64 CtBuffer::readInt64(int offset) const
{
    ctint64 result;
    ct_readType(&result, m_data, offset);
    return result;
}

ctuint64 CtBuffer::readUInt64(int offset) const
{
    ctuint64 result;
    ct_readType(&result, m_data, offset);
    return result;
}

float CtBuffer::readFloat(int offset) const
{
    float result;
    // XXX: endianness
    ct_readType(&result, m_data, offset);
    return result;
}

void CtBuffer::readChar(char *buf, int offset, int len) const
{
    if (offset < 0 || offset + len > m_size)
        return;

    memcpy(buf, m_data + offset, len);
}
