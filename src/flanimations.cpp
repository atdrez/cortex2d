#include "flanimations.h"

FlAbstractAnimation::FlAbstractAnimation()
    : m_state(Stopped),
      m_loopCount(1)
{

}

FlAbstractAnimation::~FlAbstractAnimation()
{

}

void FlAbstractAnimation::setLoopCount(int count)
{
    m_loopCount = count;
}

void FlAbstractAnimation::start()
{
    if (m_state != Running) {
        State oldState = m_state;
        m_state = Running;
        stateChanged(oldState, m_state);
    }
}

void FlAbstractAnimation::pause()
{
    if (m_state != Paused) {
        State oldState = m_state;
        m_state = Paused;
        stateChanged(oldState, m_state);
    }
}

void FlAbstractAnimation::stop()
{
    if (m_state != Stopped) {
        State oldState = m_state;
        m_state = Stopped;
        stateChanged(oldState, m_state);
    }
}

void FlAbstractAnimation::fl_advance(fluint msecs)
{
    if (m_state == Running)
        advance(msecs);
}


FlRangeAnimation::FlRangeAnimation()
    : FlAbstractAnimation(),
      m_loops(0),
      m_duration(0),
      m_progress(0),
      m_consumedTime(0)
{

}

fluint FlRangeAnimation::duration() const
{
    return m_duration;
}

void FlRangeAnimation::setDuration(fluint msecs)
{
    if (m_duration != msecs) {
        m_duration = msecs;
        updateProgress();
    }
}

FlEasingCurve FlRangeAnimation::easingCurve() const
{
    return m_easingCurve;
}

void FlRangeAnimation::setEasingCurve(const FlEasingCurve &easing)
{
    if (m_easingCurve != easing) {
        m_easingCurve = easing;
        updateProgress();
    }
}

void FlRangeAnimation::advance(fluint msecs)
{
    m_consumedTime += msecs;
    updateProgress();
}

void FlRangeAnimation::updateProgress()
{
    flreal progression = m_consumedTime / flreal(m_duration);
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

void FlRangeAnimation::stateChanged(State oldState, State newState)
{
    if (oldState == Stopped && newState == Running) {
        m_loops = 0;
        m_progress = 0.0;
        m_consumedTime = 0;
        updateProgress();
    }
}


FlNumberAnimation::FlNumberAnimation()
    : FlRangeAnimation(),
     m_startValue(0),
     m_finalValue(0),
     m_currentValue(0)
{

}

void FlNumberAnimation::setStartValue(flreal value)
{
    if (m_startValue == value) {
        m_startValue = value;
        updateCurrentValue();
    }
}

void FlNumberAnimation::setFinalValue(flreal value)
{
    if (m_finalValue != value) {
        m_finalValue = value;
        updateCurrentValue();
    }
}

void FlNumberAnimation::progressChanged(flreal progress)
{
    updateCurrentValue();
}

void FlNumberAnimation::updateCurrentValue()
{
    flreal oldValue = m_currentValue;
    m_currentValue = (m_finalValue - m_startValue) * progress() + m_startValue;

    if (m_currentValue != oldValue && m_valueChangeCallback)
        (*m_valueChangeCallback)((flreal)m_currentValue);
}

void FlNumberAnimation::setValueChangeCallback(FlFunctor *f)
{
    m_valueChangeCallback = f;
}
