QT -= gui core
LIBS = -lGLESv2 -lm -lX11 -lcortex2d
INCLUDEPATH += /usr/local/include/Cortex2D

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += sdl2
}
