QT -= gui core
LIBS = -lGLESv2 -lm -lX11 -lfluid
INCLUDEPATH += /usr/local/include/Fluid

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += sdl2
}
