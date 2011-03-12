# LibCpcEmu kernel module
INCLUDEPATH += $$PWD

HEADERS += \
    kernel/types.h \
    kernel/ioport.h \
    kernel/gatearray.h \
    kernel/memory.h \
    kernel/z80.h \
    kernel/registerset.h \
    kernel/iocontroller.h \
    kernel/z80_tables.h \
    kernel/videocontroller.h \
    kernel/screenrenderer.h \
    kernel/keyboard.h

SOURCES += \
    kernel/gatearray.cpp \
    kernel/z80.cpp \
    kernel/registerset.cpp \
    kernel/iocontroller.cpp \
    kernel/z80_p.cpp \
    kernel/memory.cpp \
    kernel/videocontroller.cpp \
    kernel/videocontroller_p.cpp \
    kernel/keyboard.cpp
