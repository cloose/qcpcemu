#-------------------------------------------------
#
# Project created by QtCreator 2011-03-19T16:54:18
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_z80_8bit_arithmetic
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_z80_8bit_arithmetic.cpp
#DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ..\..\LibCpcEmu \
               ..\..\LibCpcEmu\kernel

win32:LIBS += ../../LibCpcEmu-build-desktop/debug/LibCpcEmu.dll
