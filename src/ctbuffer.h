#ifndef CTBUFFER_H
#define CTBUFFER_H

#include "ctglobal.h"

class CtBuffer
{
public:
    CtBuffer(int size);
    ~CtBuffer();

    int size() const { return m_size; }
    ctubyte *data() const { return m_data; }

    ctbyte readByte(int offset) const;
    ctubyte readUByte(int offset) const;

    ctint16 readInt16(int offset) const;
    ctuint16 readUInt16(int offset) const;

    ctint32 readInt32(int offset) const;
    ctuint32 readUInt32(int offset) const;

    ctint64 readInt64(int offset) const;
    ctuint64 readUInt64(int offset) const;

    float readFloat(int offset) const;

    void readChar(char *, int offset, int len) const;

private:
    int m_size;
    ctubyte *m_data;
};

#endif
