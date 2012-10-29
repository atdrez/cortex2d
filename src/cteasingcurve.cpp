#include "cteasingcurve.h"
#include "3rdparty/easingfunctions.cpp"

CtEasingCurve::CtEasingCurve()
    : m_type(Linear)
{

}

CtEasingCurve::CtEasingCurve(Type type)
    : m_type(type)
{

}

ctreal CtEasingCurve::easingValue(ctreal value) const
{
    return valueForCurve(m_type, value);
}

ctreal CtEasingCurve::valueForCurve(Type type, ctreal value)
{
    switch (type) {
    case Linear:
        return easeNone(value);
    case InQuadratic:
        return quadraticEaseIn(value);
    case OutQuadratic:
        return quadraticEaseOut(value);
    case InOutQuadratic:
        return quadraticEaseInOut(value);
    case InCubic:
        return cubicEaseIn(value);
    case OutCubic:
        return cubicEaseOut(value);
    case InOutCubic:
        return cubicEaseInOut(value);
    case InExponential:
        return exponentialEaseIn(value);
    case OutExponential:
        return exponentialEaseOut(value);
    case InOutExponential:
        return exponentialEaseInOut(value);
    case InQuintic:
        return quinticEaseIn(value);
    case OutQuintic:
        return quinticEaseOut(value);
    case InOutQuintic:
        return quinticEaseInOut(value);
    case InQuartic:
        return quarticEaseIn(value);
    case OutQuartic:
        return quarticEaseOut(value);
    case InOutQuartic:
        return quarticEaseInOut(value);
    case InSinusoidal:
        return sinusoidalEaseIn(value);
    case OutSinusoidal:
        return sinusoidalEaseOut(value);
    case InOutSinusoidal:
        return sinusoidalEaseInOut(value);
    case InCircular:
        return circularEaseIn(value);
    case OutCircular:
        return circularEaseOut(value);
    case InOutCircular:
        return circularEaseInOut(value);
    }

    return value;
}
