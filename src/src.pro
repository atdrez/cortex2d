TEMPLATE = lib
TARGET = fluid

###
QT -= gui core
LIBS = -lGLESv2 -lm -lX11

DEFINES += FL_SDL_BACKEND

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += sdl2
}

HEADERS += \
    flwindow.h \
    flapplication.h \
    flevents.h \
    flmatrix.h \
    flglobal.h \
    flitem.h \
    flGL.h \
    flrect.h \
    flpoint.h \
    fltexture.h \
    flfunctor.h \
    flmap.h \
    flvector.h \
    flstring.h \
    fllist.h \
    flmath.h \
    flsceneview.h \
    fldragcursor.h \
    flshadereffect.h \
    flshaderprogram.h \
    flshaderuniform.h \
    flanimations.h \
    fleasingcurve.h \
    utils/flfile.h \
    utils/flbuffer.h \
    utils/fltgaloader.h \
    utils/flpvrloader.h \
    utils/flddsloader.h \
    3rdparty/tricollision.h \
    flopenglfunctions.h

SOURCES += \
    flwindow.cpp \
    flwindow_sdl.cpp \
    flapplication.cpp \
    flapplication_p.cpp \
    flapplication_sdl.cpp \
    flevents.cpp \
    flmatrix.cpp \
    flitem.cpp \
    fltexture.cpp \
    flsceneview.cpp \
    flshadereffect.cpp \
    flshaderprogram.cpp \
    flanimations.cpp \
    fleasingcurve.cpp \
    utils/flfile.cpp \
    utils/flbuffer.cpp \
    utils/fltgaloader.cpp \
    utils/flpvrloader.cpp \
    utils/flddsloader.cpp \
    3rdparty/tricollision.cpp \
    flopenglfunctions.cpp

include(audio/audio.pri)
include(../include/include.pri)

target.path = /usr/local/lib
headers.path = /usr/local/include/Fluid
headers.files = $$HEADERS
INSTALLS += target headers
