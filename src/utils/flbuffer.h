#ifndef FLBUFFER_H
#define FLBUFFER_H

#include "flglobal.h"

class FlBuffer
{
public:
    FlBuffer(int size);
    ~FlBuffer();

    int size() const { return m_size; }
    flubyte *data() const { return m_data; }

    flbyte readByte(int offset) const;
    flubyte readUByte(int offset) const;

    flint16 readInt16(int offset) const;
    fluint16 readUInt16(int offset) const;

    flint32 readInt32(int offset) const;
    fluint32 readUInt32(int offset) const;

    flint64 readInt64(int offset) const;
    fluint64 readUInt64(int offset) const;

    float readFloat(int offset) const;

    void readChar(char *, int offset, int len) const;

private:
    int m_size;
    flubyte *m_data;
};

#endif
