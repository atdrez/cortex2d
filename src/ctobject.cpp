#include "ctobject.h"
#include "ctevents.h"

CtObject::CtObject(CtObject *parent)
    : m_parent(parent)
{

}

CtObject::~CtObject()
{
    destroyed();
}

bool CtObject::sendEvent(CtObject *object, CtEvent *event)
{
    CT_ASSERT(object != 0 && event != 0);

    return object->event(event);
}

bool CtObject::event(CtEvent *event)
{
    CT_ASSERT(event != 0);

    event->setAccepted(false);
    return false;
}
