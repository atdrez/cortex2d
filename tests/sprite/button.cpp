#include "button.h"


Button::Button(const CtString &texturePath, CtSprite *parent)
    : FrameItem(texturePath, parent),
      m_pressed(false),
      m_enabled(true)
{
    setFlag(CtSprite::AcceptsTouchEvent, true);
}

void Button::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    setFrameKey(enabled ? m_normalFrame.data() : m_disabledFrame.data());
}

void Button::setNormalFrame(const CtString &key)
{
    m_normalFrame = key;

    if (m_enabled)
        setFrameKey(key.data());
}

void Button::setPressedFrame(const CtString &key)
{
    m_pressedFrame = key;

    if (m_enabled && m_pressed)
        setFrameKey(key.data());
}

void Button::setDisabledFrame(const CtString &key)
{
    m_disabledFrame = key;

    if (!m_enabled)
        setFrameKey(key.data());
}

void Button::touchBeginEvent(CtTouchEvent *event)
{
    if (m_enabled) {
        m_pressed = true;

        if (!m_pressedFrame.isEmpty())
            setFrameKey(m_pressedFrame.data());
    } else {
        FrameItem::touchBeginEvent(event);
    }
}

void Button::touchEndEvent(CtTouchEvent *event)
{
    const CtTouchPoint &touch = event->touchPointAt(0);

    m_pressed = false;

    if (m_enabled || m_disabledFrame.isEmpty())
        setFrameKey(m_normalFrame.data());
    else
        setFrameKey(m_disabledFrame.data());

    if (m_enabled && touch.isInsideClickThreshold())
        clicked();
}
