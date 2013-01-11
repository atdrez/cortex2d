#ifndef PLAYER_H
#define PLAYER_H

#include <Cortex2D>
#include "frameitem.h"

class Player : public FrameItem
{
public:
    enum State {
        WalkingState,
        FlyingState,
        RunToFlyState
    };

    Player(CtSceneItem *parent = 0);
    ~Player();

    State state() const;
    void setState(State state);

protected:
    void advance(ctuint ms);

private:
    State m_state;
    ctreal m_frame;
};

#endif
