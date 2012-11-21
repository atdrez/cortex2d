#ifndef CTABSTRACTANIMATION_H
#define CTABSTRACTANIMATION_H

#include "ctglobal.h"
#include "cteasingcurve.h"
#include "ctsignal.h"

class CtSceneItem;

class CtAbstractAnimation
{
public:
    enum State {
        Running,
        Paused,
        Stopped
    };

    CtAbstractAnimation();
    virtual ~CtAbstractAnimation();

    void start();
    void pause();
    void stop();
    State state() const { return m_state; }

    bool isRunning() const { return m_state == Running; }

    int loopCount() const { return m_loopCount; }
    void setLoopCount(int loopCount);

    void ct_advance(ctuint msecs);

protected:
    virtual void advance(ctuint msecs) = 0;
    virtual void stateChanged(State oldState, State newState) = 0;

private:
    State m_state;
    int m_loopCount;
};


class CtRangeAnimation : public CtAbstractAnimation
{
public:
    CtRangeAnimation();

    ctuint duration() const;
    void setDuration(ctuint msecs);

    CtEasingCurve easingCurve() const;
    void setEasingCurve(const CtEasingCurve &easing);

public:
    CtSignal0 finished;

protected:
    ctreal progress() const { return m_progress; }
    virtual void progressChanged(ctreal progress) = 0;

private:
    void updateProgress();
    void advance(ctuint msecs);
    void stateChanged(State oldState, State newState);

    int m_loops;
    ctuint m_duration;
    ctreal m_progress;
    ctuint m_consumedTime;
    CtEasingCurve m_easingCurve;
};


class CtNumberAnimation : public CtRangeAnimation
{
public:
    CtNumberAnimation();

    ctreal startValue() { return m_startValue; }
    void setStartValue(ctreal value);

    ctreal finalValue() const { return m_finalValue; }
    void setFinalValue(ctreal value);

public:
    CtSignal1<ctreal> valueChanged;

protected:
    void updateCurrentValue();
    void progressChanged(ctreal progress);

private:
    ctreal m_startValue;
    ctreal m_finalValue;
    ctreal m_currentValue;
};

#endif
