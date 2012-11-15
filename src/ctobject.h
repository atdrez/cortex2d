#ifndef CTOBJECT_H
#define CTOBJECT_H

#include "ctsignal.h"

class CtObject
{
public:
    CtObject(CtObject *parent = 0)
        : m_parent(parent) {}

    virtual ~CtObject() {
        destroyed();
    }

    CtSignal0 destroyed;

private:
    CtObject *m_parent;
};

#endif
