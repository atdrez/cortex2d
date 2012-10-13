#ifndef EVENT_H
#define EVENT_H

#include "flglobal.h"
#include "fllist.h"

class FlSceneItem;

class FlEvent
{
public:
    enum Type {
        MousePress,
        MouseMove,
        MouseRelease,
        WindowResize,
        WindowClose,
        KeyPress,
        KeyRelease,
        TouchBegin,
        TouchUpdate,
        TouchEnd,
        DragEnter,
        DragMove,
        DragLeave,
        Drop,
        DragCursorDrop,
        DragCursorCancel
    };

    FlEvent(Type type);

    Type type() const { return m_type; }

    bool isAccepted() const { return m_accepted; }
    void setAccepted(bool accepted);

private:
    Type m_type;
    bool m_accepted;
};


class FlMouseEvent : public FlEvent
{
public:
    FlMouseEvent(Type type, Fl::MouseButton button, flreal x, flreal y,
                 flreal sceneX, flreal sceneY, flreal initialX, flreal initialY);

    flreal x() const { return m_x; }
    flreal y() const { return m_y; }

    flreal sceneX() const { return m_sceneX; }
    flreal sceneY() const { return m_sceneY; }

    flreal initialX() const { return m_initialX; }
    flreal initialY() const { return m_initialY; }

    Fl::MouseButton button() const { return m_button; }

private:
    flreal m_x;
    flreal m_y;
    flreal m_sceneX;
    flreal m_sceneY;
    flreal m_initialX;
    flreal m_initialY;
    Fl::MouseButton m_button;
};


class FlDragDropEvent : public FlEvent
{
public:
    enum Operation {
        Copy = 1,
        Move = 2
    };

    FlDragDropEvent(Type type, FlSceneItem *sourceItem, FlSceneItem *targetItem,
                    FlSceneItem *draggedItem, const FlString &mime, Operation operation,
                    flreal x, flreal y, flreal sceneX, flreal sceneY);

    FlString mime() const { return m_mime; }

    FlSceneItem *sourceItem() const { return m_sourceItem; }

    FlSceneItem *targetItem() const { return m_targetItem; }

    FlSceneItem *draggedItem() const { return m_draggedItem; }

    flreal x() const { return m_x; }
    flreal y() const { return m_y; }

    flreal sceneX() const { return m_sceneX; }
    flreal sceneY() const { return m_sceneY; }

    Operation operation() const { return m_operation; }

private:
    FlSceneItem *m_sourceItem;
    FlSceneItem *m_targetItem;
    FlSceneItem *m_draggedItem;
    FlString m_mime;
    Operation m_operation;
    flreal m_x;
    flreal m_y;
    flreal m_sceneX;
    flreal m_sceneY;
};


class FlKeyEvent : public FlEvent
{
public:
    FlKeyEvent(Type type, int key, int modifiers, bool autoRepeat = false);

    int key() const { return m_key; }
    int modifiers() const { return m_modifiers; }
    int isAutoRepeat() const { return m_autoRepeat; }

private:
    int m_key;
    int m_modifiers;
    bool m_autoRepeat;
};


class FlWindowResizeEvent : public FlEvent
{
public:
    FlWindowResizeEvent(int width, int height);

    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    int m_width;
    int m_height;
};

class FlWindowCloseEvent : public FlEvent
{
public:
    FlWindowCloseEvent()
        : FlEvent(WindowClose) {}
};

class FlTouchPoint
{
public:
    FlTouchPoint();
    FlTouchPoint(int id, flreal x, flreal y, flreal initialX,
                 flreal initialY, flreal pressure, bool pressed);
    FlTouchPoint(const FlTouchPoint &p);

    int id() const { return m_id; }
    bool isValid() { return m_id >= 0; }

    flreal x() const { return m_x; }
    flreal y() const { return m_y; }

    flreal initialX() const { return m_initialX; }
    flreal initialY() const { return m_initialY; }

    bool isInsideClickThreshold() const;

    bool isPressed() const { return m_pressed; }
    flreal pressure() const { return m_pressure; }

private:
    int m_id;
    flreal m_x;
    flreal m_y;
    flreal m_initialX;
    flreal m_initialY;
    flreal m_pressure;
    bool m_pressed;
};

typedef FlList<FlTouchPoint> FlTouchPointList;


class FlTouchEvent : public FlEvent
{
public:
    FlTouchEvent(Type type, int id, const FlTouchPoint &p);
    FlTouchEvent(Type type, int id, const FlTouchPointList &points);

    int id() const { return m_id; }

    FlTouchPoint findTouchById(int id) const;

    FlTouchPoint touchPointAt(int index) const;
    int touchPointCount() const { return m_points.size(); }

    FlTouchPointList touchPoints() const { return m_points; }

private:
    int m_id;
    FlTouchPointList m_points;
};

#endif
