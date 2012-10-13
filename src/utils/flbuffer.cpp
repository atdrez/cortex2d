#include "flbuffer.h"
#include <string.h>

template<typename T>
static inline void fl_readType(T *value, flubyte *d, int i)
{
    const size_t size = sizeof(T);
    flubyte *v = reinterpret_cast<flubyte *>(value);

    FL_ASSERT(size > 8, "Invalid type size");

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


FlBuffer::FlBuffer(int size)
    : m_size(size)
{
    if (size > 0)
        m_data = new flubyte[size];
    else
        m_data = 0;
}

FlBuffer::~FlBuffer()
{
    if (m_data)
        delete [] m_data;
}

flbyte FlBuffer::readByte(int offset) const
{
    if (offset < 0 || offset >= m_size)
        return 0;

    flbyte result;
    fl_readType(&result, m_data, offset);
    return result;
}

flubyte FlBuffer::readUByte(int offset) const
{
    if (offset < 0 || offset >= m_size)
        return 0;

    flubyte result;
    fl_readType(&result, m_data, offset);
    return result;
}

flint16 FlBuffer::readInt16(int offset) const
{
    if (offset < 0 || offset >= m_size)
        return 0;

    flint16 result;
    fl_readType(&result, m_data, offset);
    return result;
}

fluint16 FlBuffer::readUInt16(int offset) const
{
    if (offset < 0 || offset >= m_size)
        return 0;

    fluint16 result;
    fl_readType(&result, m_data, offset);
    return result;
}

flint32 FlBuffer::readInt32(int offset) const
{
    flint32 result;
    fl_readType(&result, m_data, offset);
    return result;
}

fluint32 FlBuffer::readUInt32(int offset) const
{
    fluint32 result;
    fl_readType(&result, m_data, offset);
    return result;
}

flint64 FlBuffer::readInt64(int offset) const
{
    flint64 result;
    fl_readType(&result, m_data, offset);
    return result;
}

fluint64 FlBuffer::readUInt64(int offset) const
{
    fluint64 result;
    fl_readType(&result, m_data, offset);
    return result;
}

float FlBuffer::readFloat(int offset) const
{
    float result;
    // XXX: endianness
    fl_readType(&result, m_data, offset);
    return result;
}

void FlBuffer::readChar(char *buf, int offset, int len) const
{
    if (offset < 0 || offset + len > m_size)
        return;

    memcpy(buf, m_data + offset, len);
}
