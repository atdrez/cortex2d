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
    CT_ASSERT(!object || !event,
              "Invalid parameter passed to CtObject::sendEvent");

    return object->event(event);
}

bool CtObject::event(CtEvent *event)
{
    CT_ASSERT(!event, "Invalid parameter passed to CtObject::event");

    event->setAccepted(false);
    return false;
}
