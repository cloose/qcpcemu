#-------------------------------------------------
#
# Project created by QtCreator 2011-01-30T13:28:19
#
#-------------------------------------------------

QT       -= gui

TARGET = LibCpcEmu
TEMPLATE = lib

DEFINES += LIBCPCEMU_LIBRARY
HEADERS += LibCpcEmu_global.h \
    cpcsystem.h

include(kernel/kernel.pri)

SOURCES += \
    cpcsystem.cpp \
    cpcsystem_p.cpp
