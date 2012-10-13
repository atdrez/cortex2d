#ifndef FLDRAGCURSOR_H
#define FLDRAGCURSOR_H

#include "flitem.h"

class FlDragCursor
{
public:
    enum Operation {
        Copy = 1,
        Move = 2
    };

    FlDragCursor(FlSceneItem *sourceItem, FlSceneItem *draggedItem,
                 const FlString &mime, Operation operation);

    flreal x() const { return m_x; }
    void setX(flreal x) { m_x = x; }

    flreal y() const { return m_y; }
    void setY(flreal y) { m_y = y; }

    FlString mime() const { return m_mime; }

    Operation operation() const { return m_operation; }

    FlSceneItem *sourceItem() const { return m_sourceItem; }

    FlSceneItem *draggedItem() const { return m_draggedItem; }

private:
    flreal m_x;
    flreal m_y;
    FlString m_mime;
    Operation m_operation;
    FlSceneItem *m_sourceItem;
    FlSceneItem *m_draggedItem;
};


inline FlDragCursor::FlDragCursor(FlSceneItem *sourceItem, FlSceneItem *draggedItem,
                                  const FlString &mime, Operation operation)
    : m_x(0),
      m_y(0),
      m_mime(mime),
      m_operation(operation),
      m_sourceItem(sourceItem),
      m_draggedItem(draggedItem)
{

}

#endif
