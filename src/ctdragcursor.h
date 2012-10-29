#ifndef CTDRAGCURSOR_H
#define CTDRAGCURSOR_H

#include "ctitem.h"

class CtDragCursor
{
public:
    enum Operation {
        Copy = 1,
        Move = 2
    };

    CtDragCursor(CtSceneItem *sourceItem, CtSceneItem *draggedItem,
                 const CtString &mime, Operation operation);

    ctreal x() const { return m_x; }
    void setX(ctreal x) { m_x = x; }

    ctreal y() const { return m_y; }
    void setY(ctreal y) { m_y = y; }

    CtString mime() const { return m_mime; }

    Operation operation() const { return m_operation; }

    CtSceneItem *sourceItem() const { return m_sourceItem; }

    CtSceneItem *draggedItem() const { return m_draggedItem; }

private:
    ctreal m_x;
    ctreal m_y;
    CtString m_mime;
    Operation m_operation;
    CtSceneItem *m_sourceItem;
    CtSceneItem *m_draggedItem;
};


inline CtDragCursor::CtDragCursor(CtSceneItem *sourceItem, CtSceneItem *draggedItem,
                                  const CtString &mime, Operation operation)
    : m_x(0),
      m_y(0),
      m_mime(mime),
      m_operation(operation),
      m_sourceItem(sourceItem),
      m_draggedItem(draggedItem)
{

}

#endif
