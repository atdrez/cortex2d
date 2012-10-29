#ifndef CTEASINGCURVE_H
#define CTEASINGCURVE_H

#include "ctglobal.h"

class CtEasingCurve
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

    CtEasingCurve();
    CtEasingCurve(Type type);

    Type type() const { return m_type; }

    ctreal easingValue(ctreal value) const;

    bool operator==(const CtEasingCurve &e) { return e.m_type == m_type; }
    bool operator!=(const CtEasingCurve &e) { return e.m_type != m_type; }

    static ctreal valueForCurve(Type type, ctreal value);

private:
    Type m_type;
};

#endif
