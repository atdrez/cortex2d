INCLUDEPATH += $$PWD

HEADERS += \
           $$PWD/ctsoundmixer.h \
           $$PWD/ctsoundsample.h

SOURCES += \
           $$PWD/ctsoundmixer.cpp \
           $$PWD/ctsoundsample.cpp

unix {
    SOURCES += \
           $$PWD/ctsoundmixer_openal.cpp \
           $$PWD/ctsoundsample_openal.cpp
} else {
    SOURCES += \
           $$PWD/ctsoundmixer_sdl.cpp \
           $$PWD/ctsoundsample_sdl.cpp
}
