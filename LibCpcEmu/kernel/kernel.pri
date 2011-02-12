# LibCpcEmu kernel module
INCLUDEPATH += $$PWD

HEADERS += \
    kernel/types.h \
    kernel/ioport.h \
    kernel/gatearray.h \
    kernel/memory.h \
    kernel/memoryaccessdevice.h \
    kernel/z80.h

SOURCES += \
    kernel/gatearray.cpp \
    kernel/memoryaccessdevice.cpp \
    kernel/z80.cpp
