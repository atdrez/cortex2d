#ifndef CTMATH_H
#define CTMATH_H

#include <math.h>

#define CT_PI 3.14159265358979323846264338327f

template<typename T>
inline T ctClamp(T a, T b, T c)
{
    return (b < a) ? a : ((b > c) ? c : b);
}

template<typename T>
inline T ctMax(T a, T b)
{
    return (a > b) ? a : b;
}

template<typename T>
inline T ctMin(T a, T b)
{
    return (a < b) ? a : b;
}

template<typename T>
inline T ctRound(T a)
{
    return floor(a + 0.5);
}

template<typename T>
inline T ctCeil(T a)
{
    return ceil(a);
}

template<typename T>
inline T ctFloor(T a)
{
    return floor(a);
}

#define ctSqrt(a) sqrt(a)

#endif
