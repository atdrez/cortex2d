#include "player.h"


Player::Player(CtSprite *parent)
    : FrameItem("default", parent),
      m_state(WalkingState),
      m_frame(0)
{
    static const char *playerAssets[] = {
        "player/walking/1.png",
        "player/walking/2.png",
        "player/walking/3.png",
        "player/walking/4.png",
        "player/walking/5.png",
        "player/walking/6.png",
        "player/flying/1.png",
        "player/flying/2.png",
        "player/flying/3.png",
    };

    setFrameKeys(playerAssets, 9);
}

Player::~Player()
{

}

Player::State Player::state() const
{
    return m_state;
}

void Player::setState(State state)
{
    m_state = state;

    switch (m_state) {
    case WalkingState:
        m_frame = 0;
        break;
    case FlyingState:
        m_frame = 6.0;
        break;
    default:
        break;
    }
}

void Player::advance(ctuint ms)
{
    m_frame += ms / 90.0f;

    switch (m_state) {
    case WalkingState:
        if (m_frame >= 6.0)
            m_frame -= 6.0;
        break;
    case FlyingState:
        if (m_frame >= 9.0)
            m_frame -= 3.0;
        break;
    default:
        return;
    }

    setFrame(m_frame);
}
