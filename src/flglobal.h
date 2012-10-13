#ifndef FLGLOBAL_H
#define FLGLOBAL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "flstring.h"

#define FL_DEBUG_MODE // XXX: remove on release
// XXX: adjust mesa
//#define FL_ANDROID
//#define FL_OPENGL_ES2

#define FL_SIMULATE_TOUCH


#ifdef FL_ANDROID
#    include <sstream>
#    include <android/log.h>
#endif


namespace Fl
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
        PVRTextureFile,
        DDSTextureFile
    };
};

typedef uint32_t fluint;
typedef float flreal;
typedef char flbyte;
typedef unsigned char flubyte;
typedef int16_t flint16;
typedef int32_t flint32;
typedef int64_t flint64;
typedef uint16_t fluint16;
typedef uint32_t fluint32;
typedef uint64_t fluint64;

#define flsrand(t) srand(t)
#define flrand(t) rand(t)

#define FL_D(T) T##Private *d = static_cast<T##Private *>(d_ptr)
#define FL_Q(T) T *q = q_ptr

#define FL_UNUSED(a) (void)a;

#define FL_ENDL std::endl

#define foreach(t, c) \
    for (typeof(c.begin()) __fl_ittt = c.begin(); __fl_ittt != c.end(); __fl_ittt++) \
        if (bool ok = true) for (t = *(__fl_ittt); ok; ok = false)

#define foreach_reverse(t, c) \
    for (typeof(c.rbegin()) __fl_ittt = c.rbegin(); __fl_ittt != c.rend(); __fl_ittt++) \
        if (bool ok = true) for (t = *(__fl_ittt); ok; ok = false)

#ifndef FL_ANDROID
#    define FL_WARNING(msg) { std::cerr << msg << std::endl; }
#else
#    define FL_WARNING(msg) { std::ostringstream __fl_dbg_msg; __fl_dbg_msg << msg << std::endl; \
        __android_log_print(ANDROID_LOG_VERBOSE, "FL_DEBUG", __fl_dbg_msg.str().c_str()); }
#endif

#define FL_FATAL(msg) { FL_WARNING(msg); exit(-1); }

#define FL_CHECK_GL_ERROR(code) {                                       \
        GLenum _glErr = FlGL::glGetError();                             \
        if (_glErr != GL_NO_ERROR)                                      \
        { FL_WARNING("GLError: " << FlGL::getGlErrorMessage(_glErr)); code; } }

#ifdef FL_DEBUG_MODE

#    ifndef FL_ANDROID
#        define FL_DEBUG(msg) { std::clog << msg << std::endl; }
#    else
#        define FL_DEBUG(msg) { std::ostringstream __fl_dbg_msg; __fl_dbg_msg << msg << std::endl; \
        __android_log_print(ANDROID_LOG_VERBOSE, "FL_WARNING", __fl_dbg_msg.str().c_str()); }
#    endif

#    define FL_ASSERT(e, msg) if (e) { FL_FATAL(msg); }

#    define FL_GL_DEBUG_CHECK() {                                      \
        GLenum _glErr = FlGL::glGetError();                            \
        if (_glErr != GL_NO_ERROR)                                      \
            FL_WARNING("GLError: " << FlGL::getGlErrorMessage(_glErr)); }
#else
#    define FL_DEBUG(x)

#    define FL_ASSERT(e, msg)

#    define FL_GL_DEBUG_CHECK()
#endif

#endif
