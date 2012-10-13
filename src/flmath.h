#ifndef FLMATH_H
#define FLMATH_H

#include <math.h>

#define FL_PI 3.14159265358979323846264338327f

template<typename T>
inline T flClamp(T a, T b, T c)
{
    return (b < a) ? a : ((b > c) ? c : b);
}

template<typename T>
inline T flMax(T a, T b)
{
    return (a > b) ? a : b;
}

template<typename T>
inline T flMin(T a, T b)
{
    return (a < b) ? a : b;
}

#define flSqrt(a) sqrt(a)

#endif
