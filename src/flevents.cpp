#include "flevents.h"

static const flreal fl_clickThreshold = 10;


FlEvent::FlEvent(Type type)
    : m_type(type),
      m_accepted(false)
{

}

void FlEvent::setAccepted(bool accepted)
{
    m_accepted = accepted;
}

FlDragDropEvent::FlDragDropEvent(Type type, FlSceneItem *sourceItem, FlSceneItem *targetItem,
                                 FlSceneItem *draggedItem, const FlString &mime,
                                 Operation operation, flreal x, flreal y,
                                 flreal sceneX, flreal sceneY)
    : FlEvent(type),
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

FlMouseEvent::FlMouseEvent(Type type, Fl::MouseButton button,
                           flreal x, flreal y, flreal sceneX, flreal sceneY,
                           flreal initialX, flreal initialY)
    : FlEvent(type),
      m_x(x),
      m_y(y),
      m_sceneX(sceneX),
      m_sceneY(sceneY),
      m_initialX(initialX),
      m_initialY(initialY),
      m_button(button)
{

}

FlKeyEvent::FlKeyEvent(Type type, int key, const int modifiers, bool autoRepeat)
    : FlEvent(type),
      m_key(key),
      m_modifiers(modifiers),
      m_autoRepeat(autoRepeat)
{

}


FlWindowResizeEvent::FlWindowResizeEvent(int width, int height)
    : FlEvent(WindowResize),
      m_width(width),
      m_height(height)
{

}

FlTouchPoint::FlTouchPoint()
    : m_id(-1),
      m_x(0),
      m_y(0),
      m_initialX(0),
      m_initialY(0),
      m_pressure(0),
      m_pressed(0)
{

}

FlTouchPoint::FlTouchPoint(int id, flreal x, flreal y, flreal initialX,
                           flreal initialY, flreal pressure, bool pressed)
    : m_id(id),
      m_x(x),
      m_y(y),
      m_initialX(initialX),
      m_initialY(initialY),
      m_pressure(pressure),
      m_pressed(pressed)
{

}

FlTouchPoint::FlTouchPoint(const FlTouchPoint &p)
    : m_id(p.m_id),
      m_x(p.m_x),
      m_y(p.m_y),
      m_initialX(p.m_initialX),
      m_initialY(p.m_initialY),
      m_pressure(p.m_pressure),
      m_pressed(p.m_pressed)
{

}

bool FlTouchPoint::isInsideClickThreshold() const
{
    const flreal dx = (m_initialX - m_x);
    const flreal dy = (m_initialY - m_y);
    return (dx * dx + dy * dy) < (fl_clickThreshold * fl_clickThreshold);
}


FlTouchEvent::FlTouchEvent(Type type, int id, const FlTouchPoint &p)
    :  FlEvent(type),
       m_id(id)
{
    m_points.push_back(p);
}

FlTouchEvent::FlTouchEvent(Type type, int id, const FlTouchPointList &points)
    : FlEvent(type),
      m_id(id),
      m_points(points)
{

}

FlTouchPoint FlTouchEvent::touchPointAt(int index) const
{
    if (index < 0 || index >= m_points.size())
        return FlTouchPoint();

    // XXX: should be a std::vector
    int i = 0;
    foreach (const FlTouchPoint &p, m_points) {
        if (index == i++)
            return p;
    }

    return FlTouchPoint();
}

FlTouchPoint FlTouchEvent::findTouchById(int id) const
{
    foreach (const FlTouchPoint &p, m_points) {
        if (p.id() == id)
            return p;
    }

    return FlTouchPoint();
}
