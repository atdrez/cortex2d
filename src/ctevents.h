#ifndef EVENT_H
#define EVENT_H

#include "ctglobal.h"
#include "ctlist.h"

class CtSceneItem;

class CtEvent
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
        DragCursorCancel,
        WindowMinimize,
        WindowRestore,
        ApplicationReady,
        ApplicationRelease,
        Custom = 0x80
    };

    CtEvent(Type type);

    Type type() const { return m_type; }

    bool isAccepted() const { return m_accepted; }
    void setAccepted(bool accepted);

private:
    Type m_type;
    bool m_accepted;
};


class CtWindowMinimizeEvent : public CtEvent
{
public:
    CtWindowMinimizeEvent()
        : CtEvent(WindowMinimize) {}
};


class CtWindowRestoreEvent : public CtEvent
{
public:
    CtWindowRestoreEvent()
        : CtEvent(WindowRestore) {}
};


class CtMouseEvent : public CtEvent
{
public:
    CtMouseEvent(Type type, Ct::MouseButton button, ctreal x, ctreal y,
                 ctreal sceneX, ctreal sceneY, ctreal initialX, ctreal initialY);

    ctreal x() const { return m_x; }
    ctreal y() const { return m_y; }

    ctreal sceneX() const { return m_sceneX; }
    ctreal sceneY() const { return m_sceneY; }

    ctreal initialX() const { return m_initialX; }
    ctreal initialY() const { return m_initialY; }

    Ct::MouseButton button() const { return m_button; }

private:
    ctreal m_x;
    ctreal m_y;
    ctreal m_sceneX;
    ctreal m_sceneY;
    ctreal m_initialX;
    ctreal m_initialY;
    Ct::MouseButton m_button;
};


class CtDragDropEvent : public CtEvent
{
public:
    enum Operation {
        Copy = 1,
        Move = 2
    };

    CtDragDropEvent(Type type, CtSceneItem *sourceItem, CtSceneItem *targetItem,
                    CtSceneItem *draggedItem, const CtString &mime, Operation operation,
                    ctreal x, ctreal y, ctreal sceneX, ctreal sceneY);

    CtString mime() const { return m_mime; }

    CtSceneItem *sourceItem() const { return m_sourceItem; }

    CtSceneItem *targetItem() const { return m_targetItem; }

    CtSceneItem *draggedItem() const { return m_draggedItem; }

    ctreal x() const { return m_x; }
    ctreal y() const { return m_y; }

    ctreal sceneX() const { return m_sceneX; }
    ctreal sceneY() const { return m_sceneY; }

    Operation operation() const { return m_operation; }

private:
    CtSceneItem *m_sourceItem;
    CtSceneItem *m_targetItem;
    CtSceneItem *m_draggedItem;
    CtString m_mime;
    Operation m_operation;
    ctreal m_x;
    ctreal m_y;
    ctreal m_sceneX;
    ctreal m_sceneY;
};


class CtKeyEvent : public CtEvent
{
public:
    CtKeyEvent(Type type, int key, int modifiers, bool autoRepeat = false);

    int key() const { return m_key; }
    int modifiers() const { return m_modifiers; }
    int isAutoRepeat() const { return m_autoRepeat; }

private:
    int m_key;
    int m_modifiers;
    bool m_autoRepeat;
};


class CtWindowResizeEvent : public CtEvent
{
public:
    CtWindowResizeEvent(int width, int height);

    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    int m_width;
    int m_height;
};

class CtWindowCloseEvent : public CtEvent
{
public:
    CtWindowCloseEvent()
        : CtEvent(WindowClose) {}
};

class CtTouchPoint
{
public:
    CtTouchPoint();
    CtTouchPoint(int id, ctreal x, ctreal y, ctreal initialX,
                 ctreal initialY, ctreal pressure, bool pressed);
    CtTouchPoint(const CtTouchPoint &p);

    int id() const { return m_id; }
    bool isValid() { return m_id >= 0; }

    ctreal x() const { return m_x; }
    ctreal y() const { return m_y; }

    ctreal initialX() const { return m_initialX; }
    ctreal initialY() const { return m_initialY; }

    bool isInsideClickThreshold() const;

    bool isPressed() const { return m_pressed; }
    ctreal pressure() const { return m_pressure; }

private:
    int m_id;
    ctreal m_x;
    ctreal m_y;
    ctreal m_initialX;
    ctreal m_initialY;
    ctreal m_pressure;
    bool m_pressed;
};

typedef CtList<CtTouchPoint> CtTouchPointList;


class CtTouchEvent : public CtEvent
{
public:
    CtTouchEvent(Type type, int id, const CtTouchPoint &p);
    CtTouchEvent(Type type, int id, const CtTouchPointList &points);

    int id() const { return m_id; }

    CtTouchPoint findTouchById(int id) const;

    CtTouchPoint touchPointAt(int index) const;
    int touchPointCount() const { return m_points.size(); }

    CtTouchPointList touchPoints() const { return m_points; }

private:
    int m_id;
    CtTouchPointList m_points;
};

#endif
