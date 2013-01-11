#include "button.h"


Button::Button(const CtString &texturePath, CtSceneItem *parent)
    : FrameItem(texturePath, parent),
      m_pressed(false),
      m_enabled(true)
{
    setFlag(CtSceneItem::AcceptsTouchEvent, true);
}

void Button::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    setFrameKey(enabled ? m_normalFrame.c_str() : m_disabledFrame.c_str());
}

void Button::setNormalFrame(const CtString &key)
{
    m_normalFrame = key;

    if (m_enabled)
        setFrameKey(key.c_str());
}

void Button::setPressedFrame(const CtString &key)
{
    m_pressedFrame = key;

    if (m_enabled && m_pressed)
        setFrameKey(key.c_str());
}

void Button::setDisabledFrame(const CtString &key)
{
    m_disabledFrame = key;

    if (!m_enabled)
        setFrameKey(key.c_str());
}

void Button::touchBeginEvent(CtTouchEvent *event)
{
    if (m_enabled) {
        m_pressed = true;

        if (!m_pressedFrame.empty())
            setFrameKey(m_pressedFrame.c_str());
    } else {
        FrameItem::touchBeginEvent(event);
    }
}

void Button::touchEndEvent(CtTouchEvent *event)
{
    const CtTouchPoint &touch = event->touchPointAt(0);

    m_pressed = false;

    if (m_enabled || m_disabledFrame.empty())
        setFrameKey(m_normalFrame.c_str());
    else
        setFrameKey(m_disabledFrame.c_str());

    if (m_enabled && touch.isInsideClickThreshold())
        clicked();
}
