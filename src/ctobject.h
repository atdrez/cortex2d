#ifndef CTOBJECT_H
#define CTOBJECT_H

#include "ctsignal.h"

class CtEvent;

class CtObject
{
public:
    CtObject(CtObject *parent = 0);
    virtual ~CtObject();

    static bool sendEvent(CtObject *object, CtEvent *event);

public:
    CtSignal0 destroyed;

protected:
    virtual bool event(CtEvent *event);

private:
    CtObject *m_parent;
};

#endif
