#ifndef FLEASINGCURVE_H
#define FLEASINGCURVE_H

#include "flglobal.h"

class FlEasingCurve
{
public:
    enum Type {
        Linear,
        InCubic,
        OutCubic,
        InOutCubic,
        InQuadratic,
        OutQuadratic,
        InOutQuadratic,
        InSinusoidal,
        OutSinusoidal,
        InOutSinusoidal,
        InExponential,
        OutExponential,
        InOutExponential,
        InCircular,
        OutCircular,
        InOutCircular,
        InQuartic,
        OutQuartic,
        InOutQuartic,
        InQuintic,
        OutQuintic,
        InOutQuintic
    };

    FlEasingCurve();
    FlEasingCurve(Type type);

    Type type() const { return m_type; }

    flreal easingValue(flreal value) const;

    bool operator==(const FlEasingCurve &e) { return e.m_type == m_type; }
    bool operator!=(const FlEasingCurve &e) { return e.m_type != m_type; }

    static flreal valueForCurve(Type type, flreal value);

private:
    Type m_type;
};

#endif
