#include "ctanimations.h"

CtAbstractAnimation::CtAbstractAnimation()
    : m_state(Stopped),
      m_loopCount(1)
{

}

CtAbstractAnimation::~CtAbstractAnimation()
{

}

void CtAbstractAnimation::setLoopCount(int count)
{
    m_loopCount = count;
}

void CtAbstractAnimation::start()
{
    if (m_state != Running) {
        State oldState = m_state;
        m_state = Running;
        stateChanged(oldState, m_state);
    }
}

void CtAbstractAnimation::pause()
{
    if (m_state != Paused) {
        State oldState = m_state;
        m_state = Paused;
        stateChanged(oldState, m_state);
    }
}

void CtAbstractAnimation::stop()
{
    if (m_state != Stopped) {
        State oldState = m_state;
        m_state = Stopped;
        stateChanged(oldState, m_state);
    }
}

void CtAbstractAnimation::ct_advance(ctuint msecs)
{
    if (m_state == Running)
        advance(msecs);
}


CtRangeAnimation::CtRangeAnimation()
    : CtAbstractAnimation(),
      m_loops(0),
      m_duration(0),
      m_progress(0),
      m_consumedTime(0)
{

}

ctuint CtRangeAnimation::duration() const
{
    return m_duration;
}

void CtRangeAnimation::setDuration(ctuint msecs)
{
    if (m_duration != msecs) {
        m_duration = msecs;
        updateProgress();
    }
}

CtEasingCurve CtRangeAnimation::easingCurve() const
{
    return m_easingCurve;
}

void CtRangeAnimation::setEasingCurve(const CtEasingCurve &easing)
{
    if (m_easingCurve != easing) {
        m_easingCurve = easing;
        updateProgress();
    }
}

void CtRangeAnimation::advance(ctuint msecs)
{
    m_consumedTime += msecs;
    updateProgress();
}

void CtRangeAnimation::updateProgress()
{
    ctreal progression = m_consumedTime / ctreal(m_duration);
    bool underLimit = (progression < 1.0);

    if (!underLimit)
        progression = 1.0;

    m_progress = m_easingCurve.easingValue(progression);

    if (underLimit) {
        progressChanged(m_progress);
    } else {
        progressChanged(m_progress);

        if (loopCount() < 0) {
            m_consumedTime -= m_duration;
        } else if (m_loops < loopCount() - 1) {
            m_loops++;
            m_consumedTime -= m_duration;
        } else {
            stop();
        }
    }
}

void CtRangeAnimation::stateChanged(State oldState, State newState)
{
    if (oldState == Stopped && newState == Running) {
        m_loops = 0;
        m_progress = 0.0;
        m_consumedTime = 0;
        updateProgress();
    }
}


CtNumberAnimation::CtNumberAnimation()
    : CtRangeAnimation(),
     m_startValue(0),
     m_finalValue(0),
     m_currentValue(0)
{

}

void CtNumberAnimation::setStartValue(ctreal value)
{
    if (m_startValue == value) {
        m_startValue = value;
        updateCurrentValue();
    }
}

void CtNumberAnimation::setFinalValue(ctreal value)
{
    if (m_finalValue != value) {
        m_finalValue = value;
        updateCurrentValue();
    }
}

void CtNumberAnimation::progressChanged(ctreal progress)
{
    updateCurrentValue();
}

void CtNumberAnimation::updateCurrentValue()
{
    ctreal oldValue = m_currentValue;
    m_currentValue = (m_finalValue - m_startValue) * progress() + m_startValue;

    if (m_currentValue != oldValue && m_valueChangeCallback)
        (*m_valueChangeCallback)((ctreal)m_currentValue);
}

void CtNumberAnimation::setValueChangeCallback(CtFunctor *f)
{
    m_valueChangeCallback = f;
}
