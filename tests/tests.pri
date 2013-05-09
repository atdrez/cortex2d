QT -= gui core
LIBS = -lGLESv2 -lm -lX11 -lcortex2d

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += sdl2
}
