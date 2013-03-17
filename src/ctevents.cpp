#include "ctevents.h"

static const ctreal ct_clickThreshold = 10;


CtEvent::CtEvent(Type type)
    : m_type(type),
      m_accepted(false)
{

}

void CtEvent::setAccepted(bool accepted)
{
    m_accepted = accepted;
}

CtDragDropEvent::CtDragDropEvent(Type type, CtSprite *sourceItem, CtSprite *targetItem,
                                 CtSprite *draggedItem, const CtString &mime,
                                 Operation operation, ctreal x, ctreal y,
                                 ctreal sceneX, ctreal sceneY)
    : CtEvent(type),
      m_sourceItem(sourceItem),
      m_targetItem(targetItem),
      m_draggedItem(draggedItem),
      m_mime(mime),
      m_operation(operation),
      m_x(x),
      m_y(y),
      m_sceneX(sceneX),
      m_sceneY(sceneY)
{

}

CtMouseEvent::CtMouseEvent(Type type, Ct::MouseButton button,
                           ctreal x, ctreal y, ctreal sceneX, ctreal sceneY,
                           ctreal initialX, ctreal initialY)
    : CtEvent(type),
      m_x(x),
      m_y(y),
      m_sceneX(sceneX),
      m_sceneY(sceneY),
      m_initialX(initialX),
      m_initialY(initialY),
      m_button(button)
{

}

CtKeyEvent::CtKeyEvent(Type type, int key, const int modifiers, bool autoRepeat)
    : CtEvent(type),
      m_key(key),
      m_modifiers(modifiers),
      m_autoRepeat(autoRepeat)
{

}


CtWindowResizeEvent::CtWindowResizeEvent(int width, int height)
    : CtEvent(WindowResize),
      m_width(width),
      m_height(height)
{

}

CtTouchPoint::CtTouchPoint()
    : m_id(-1),
      m_x(0),
      m_y(0),
      m_initialX(0),
      m_initialY(0),
      m_pressure(0),
      m_pressed(0)
{

}

CtTouchPoint::CtTouchPoint(int id, ctreal x, ctreal y, ctreal initialX,
                           ctreal initialY, ctreal pressure, bool pressed)
    : m_id(id),
      m_x(x),
      m_y(y),
      m_initialX(initialX),
      m_initialY(initialY),
      m_pressure(pressure),
      m_pressed(pressed)
{

}

CtTouchPoint::CtTouchPoint(const CtTouchPoint &p)
    : m_id(p.m_id),
      m_x(p.m_x),
      m_y(p.m_y),
      m_initialX(p.m_initialX),
      m_initialY(p.m_initialY),
      m_pressure(p.m_pressure),
      m_pressed(p.m_pressed)
{

}

bool CtTouchPoint::isInsideClickThreshold() const
{
    const ctreal dx = (m_initialX - m_x);
    const ctreal dy = (m_initialY - m_y);
    return (dx * dx + dy * dy) < (ct_clickThreshold * ct_clickThreshold);
}


CtTouchEvent::CtTouchEvent(Type type, int id, const CtTouchPoint &p)
    :  CtEvent(type),
       m_id(id)
{
    m_points.append(p);
}

CtTouchEvent::CtTouchEvent(Type type, int id, const CtTouchPointList &points)
    : CtEvent(type),
      m_id(id),
      m_points(points)
{

}

CtTouchPoint CtTouchEvent::touchPointAt(int index) const
{
    if (index < 0 || index >= (int)m_points.size())
        return CtTouchPoint();

    // XXX: should be a std::vector
    int i = 0;
    foreach (const CtTouchPoint &p, m_points) {
        if (index == i++)
            return p;
    }

    return CtTouchPoint();
}

CtTouchPoint CtTouchEvent::findTouchById(int id) const
{
    foreach (const CtTouchPoint &p, m_points) {
        if (p.id() == id)
            return p;
    }

    return CtTouchPoint();
}
