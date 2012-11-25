TEMPLATE = lib
TARGET = cortex2d

###
QT -= gui core
LIBS = -lGLESv2 -lm -lX11 -lpng

DEFINES += CT_SDL_BACKEND

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += sdl2 libpng
}

unix {
    PKGCONFIG += openal
    DEFINES += CT_OPENAL_AUDIO
    include(3rdparty/alut/alut.pri)
} else {
    DEFINES += CT_SDL_AUDIO
}

HEADERS += \
    cortex2d.h \
    ctwindow.h \
    ctapplication.h \
    ctbasicapplication.h \
    ctevents.h \
    ctmatrix.h \
    ctglobal.h \
    ctitem.h \
    ctGL.h \
    ctrect.h \
    ctpoint.h \
    ctsignal.h \
    cttexture.h \
    ctmap.h \
    ctvector.h \
    ctstring.h \
    ctlist.h \
    ctmath.h \
    ctobject.h \
    ctrenderer.h \
    ctsceneview.h \
    ctdragcursor.h \
    ctshadereffect.h \
    ctshaderprogram.h \
    ctshaderuniform.h \
    ctanimations.h \
    cteasingcurve.h \
    utils/ctfile.h \
    utils/ctbuffer.h \
    utils/cttgaloader.h \
    utils/ctpngloader.h \
    utils/ctpvrloader.h \
    utils/ctddsloader.h \
    3rdparty/tricollision.h \
    ctopenglfunctions.h

SOURCES += \
    ctwindow.cpp \
    ctwindow_sdl.cpp \
    ctapplication.cpp \
    ctapplication_p.cpp \
    ctapplication_sdl.cpp \
    ctevents.cpp \
    ctmatrix.cpp \
    ctitem.cpp \
    ctobject.cpp \
    cttexture.cpp \
    ctrenderer.cpp \
    ctsceneview.cpp \
    ctshadereffect.cpp \
    ctshaderprogram.cpp \
    ctanimations.cpp \
    cteasingcurve.cpp \
    utils/ctfile.cpp \
    utils/ctbuffer.cpp \
    utils/cttgaloader.cpp \
    utils/ctpngloader.cpp \
    utils/ctpvrloader.cpp \
    utils/ctddsloader.cpp \
    3rdparty/tricollision.cpp \
    ctopenglfunctions.cpp


INCLUDEPATH += ./utils

include(audio/audio.pri)
include(../include/include.pri)

target.path = /usr/local/lib
headers.path = /usr/local/include/Cortex2D
headers.files = $$HEADERS
INSTALLS += target headers
