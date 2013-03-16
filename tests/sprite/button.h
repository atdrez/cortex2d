#ifndef BUTTON_H
#define BUTTON_H

#include <Cortex2D>
#include "frameitem.h"

class Button : public FrameItem
{
public:
    Button(const CtString &texturePath, CtSprite *parent = 0);

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    void setNormalFrame(const CtString &key);
    void setPressedFrame(const CtString &key);
    void setDisabledFrame(const CtString &key);

public:
    CtSignal0 clicked;

protected:
    void touchBeginEvent(CtTouchEvent *event);
    void touchEndEvent(CtTouchEvent *event);

private:
    bool m_pressed;
    bool m_enabled;
    CtString m_normalFrame;
    CtString m_pressedFrame;
    CtString m_disabledFrame;
};

#endif
