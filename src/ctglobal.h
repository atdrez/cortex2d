#ifndef CTGLOBAL_H
#define CTGLOBAL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "ctstring.h"

#define CT_DEBUG_MODE // XXX: remove on release
// XXX: adjust mesa
//#define CT_ANDROID
//#define CT_OPENGL_ES2
//#define CT_RETINA_DISPLAY

#define CT_SIMULATE_TOUCH


#ifdef CT_ANDROID
#    include <sstream>
#    include <android/log.h>
#endif


namespace Ct
{
    enum MouseButton {
        NoButton,
        ButtonLeft,
        ButtonRight,
        ButtonMiddle
    };

    enum KeyModifier {
        NoModifier = 0x00000000,
        ShiftModifier = 0x02000000,
        ControlModifier = 0x04000000,
        AltModifier = 0x08000000,
        MetaModifier = 0x10000000,
        KeypadModifier = 0x20000000,
    };

    enum TextureFileType {
        TGATextureFile,
        PNGTextureFile,
        PVRTextureFile,
        DDSTextureFile
    };
};

typedef uint32_t ctuint;
typedef float ctreal;
typedef char ctbyte;
typedef unsigned char ctubyte;
typedef int16_t ctint16;
typedef int32_t ctint32;
typedef int64_t ctint64;
typedef uint16_t ctuint16;
typedef uint32_t ctuint32;
typedef uint64_t ctuint64;

#define CT_PRIVATE_COPY(T) private: T(const T &); T &operator=(const T &);

#define ctSrand(t) srand(t)
#define ctRand(t) rand(t)

#define CT_D(T) T##Private *d = static_cast<T##Private *>(d_ptr)
#define CT_Q(T) T *q = q_ptr

#define CT_UNUSED(a) (void)a;

#define CT_ENDL std::endl

#define foreach(t, c) \
    for (typeof(c.begin()) __ct_ittt = c.begin(); __ct_ittt != c.end(); __ct_ittt++) \
        if (bool ok = true) for (t = *(__ct_ittt); ok; ok = false)

#define foreach_reverse(t, c) \
    for (typeof(c.rbegin()) __ct_ittt = c.rbegin(); __ct_ittt != c.rend(); __ct_ittt++) \
        if (bool ok = true) for (t = *(__ct_ittt); ok; ok = false)

#ifndef CT_ANDROID
#    define CT_WARNING(msg) { std::cerr << msg << std::endl; }
#else
#    define CT_WARNING(msg) { std::ostringstream __ct_dbg_msg; __ct_dbg_msg << msg << std::endl; \
        __android_log_print(ANDROID_LOG_VERBOSE, "CT_DEBUG", __ct_dbg_msg.str().c_str()); }
#endif

#define CT_FATAL(msg) { CT_WARNING(msg); exit(-1); }

#define CT_CHECK_GL_ERROR(code) {                                       \
        GLenum _glErr = CtGL::glGetError();                             \
        if (_glErr != GL_NO_ERROR)                                      \
        { CT_WARNING("GLError: " << CtGL::getGlErrorMessage(_glErr)); code; } }

#ifdef CT_DEBUG_MODE

#    ifndef CT_ANDROID
#        define CT_DEBUG(msg) { std::clog << msg << std::endl; }
#    else
#        define CT_DEBUG(msg) { std::ostringstream __ct_dbg_msg; __ct_dbg_msg << msg << std::endl; \
        __android_log_print(ANDROID_LOG_VERBOSE, "CT_WARNING", __ct_dbg_msg.str().c_str()); }
#    endif

#    define CT_ASSERT(e) assert(e);

#    define CT_GL_DEBUG_CHECK() {                                      \
        GLenum _glErr = CtGL::glGetError();                            \
        if (_glErr != GL_NO_ERROR)                                      \
            CT_WARNING("GLError: " << CtGL::getGlErrorMessage(_glErr)); }
#else

#    define CT_DEBUG(x)

#    define CT_ASSERT(e)

#    define CT_GL_DEBUG_CHECK()
#endif

#endif
