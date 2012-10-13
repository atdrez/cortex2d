#ifndef FLABSTRACTANIMATION_H
#define FLABSTRACTANIMATION_H

#include "flglobal.h"
#include "fleasingcurve.h"
#include "flfunctor.h"

class FlSceneItem;

class FlAbstractAnimation
{
public:
    enum State {
        Running,
        Paused,
        Stopped
    };

    FlAbstractAnimation();
    virtual ~FlAbstractAnimation();

    void start();
    void pause();
    void stop();
    State state() const { return m_state; }

    int loopCount() const { return m_loopCount; }
    void setLoopCount(int loopCount);

    void fl_advance(fluint msecs);

protected:
    virtual void advance(fluint msecs) = 0;
    virtual void stateChanged(State oldState, State newState) = 0;

private:

    State m_state;
    int m_loopCount;
};


class FlRangeAnimation : public FlAbstractAnimation
{
public:
    FlRangeAnimation();

    fluint duration() const;
    void setDuration(fluint msecs);

    FlEasingCurve easingCurve() const;
    void setEasingCurve(const FlEasingCurve &easing);

protected:
    flreal progress() const { return m_progress; }
    virtual void progressChanged(flreal progress) = 0;

private:
    void updateProgress();
    void advance(fluint msecs);
    void stateChanged(State oldState, State newState);

    int m_loops;
    fluint m_duration;
    flreal m_progress;
    fluint m_consumedTime;
    FlEasingCurve m_easingCurve;
};


class FlNumberAnimation : public FlRangeAnimation
{
public:
    FlNumberAnimation();

    flreal startValue() { return m_startValue; }
    void setStartValue(flreal value);

    flreal finalValue() const { return m_finalValue; }
    void setFinalValue(flreal value);

    void setValueChangeCallback(FlFunctor *f);

protected:
    void updateCurrentValue();
    void progressChanged(flreal progress);

private:
    flreal m_startValue;
    flreal m_finalValue;
    flreal m_currentValue;
    FlFunctor *m_valueChangeCallback;
};

#endif
