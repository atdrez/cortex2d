/*
  TERMS OF USE - EASING EQUATIONS

  Open source under the BSD License.

  Copyright © 2001 Robert Penner
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

  Neither the name of the author nor the names of contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "flglobal.h"
#include "flmath.h"


inline static flreal easeNone(flreal td)
{
    return td;
}

inline static flreal quadraticEaseIn(flreal td)
{
    return td * td;
}

inline static flreal quadraticEaseOut(flreal td)
{
    return -td * (td - 2);
}

inline static flreal quadraticEaseInOut(flreal td)
{
    td *= 2.0f;

    if (td < 1)
        return 0.5f * td * td;

    td--;
    return -0.5 * (td * (td - 2) - 1);
}

inline static flreal cubicEaseIn(flreal td)
{
    return (td * td * td);
}

inline static flreal cubicEaseOut(flreal td)
{
    td -= 1.0f;
    return (td * td * td + 1);
}

inline static flreal cubicEaseInOut(flreal td)
{
    td *= 2.0f;

    if (td < 1)
        return 0.5f * td * td * td;

    td -= 2.0f;
    return 0.5 * (td * td * td + 2);
}

inline static flreal quarticEaseIn(flreal td)
{
    return td * td * td * td;
}

inline static flreal quarticEaseOut(flreal td)
{
    td -= 1.0f;
    return -(td * td * td * td - 1);
}

inline static flreal quarticEaseInOut(flreal td)
{
    td *= 2.0f;

    if (td < 1)
        return 0.5f * td * td * td * td;

    td -= 2.0f;
    return -0.5f * (td * td * td * td - 2);
}

inline static flreal quinticEaseIn(flreal td)
{
    return td * td * td * td * td;
}

inline static flreal quinticEaseOut(flreal td)
{
    td -= 1.0f;
    return (td * td * td * td * td + 1);
}

inline static flreal quinticEaseInOut(flreal td)
{
    td *= 2.0f;

    if (td < 1)
        return 0.5f * td * td * td * td * td;

    td -= 2.0f;
    return 0.5f * (td * td * td * td * td + 2);
}

inline static flreal sinusoidalEaseIn(flreal td)
{
    return 1.0f - cos(td * (FL_PI / 2.0f));
}

inline static flreal sinusoidalEaseOut(flreal td)
{
    return sin(td * (FL_PI / 2.0f));
}

inline static flreal sinusoidalEaseInOut(flreal td)
{
    return -0.5f * (cos(FL_PI * td) - 1.0f);
}

inline static flreal exponentialEaseIn(flreal td)
{
    return (td == 0.0f || td == 1.0f) ? td
        : pow(2.0f, 10.0f * (td - 1));
}

inline static flreal exponentialEaseOut(flreal td)
{
    return (td == 0.0f || td == 1.0f) ? td
        : (-pow(2.0f, -10.0f * td) + 1);
}

inline static flreal exponentialEaseInOut(flreal td)
{
    td *= 2.0f;

    if (td < 1)
        return 0.5f * pow(2.0f, 10.0f * (td - 1));

    td -= 1.0f;
    return 0.5f * (-pow(2.0f, -10.0f * td) + 2.0f);
}

inline static flreal circularEaseIn(flreal td)
{
    return -(flSqrt(1.0f - td * td) - 1.0f);
}

inline static flreal circularEaseOut(flreal td)
{
    td -= 1.0f;
    return flSqrt(1.0f - td * td);
}

inline static flreal circularEaseInOut(flreal td)
{
    td *= 2.0f;

    if (td < 1)
        return -0.5f * (flSqrt(1.0f - td * td) - 1);

    td -= 2.0f;
    return 0.5f * (flSqrt(1.0f - td * td) + 1);
}
