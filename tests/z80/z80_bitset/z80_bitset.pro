#-------------------------------------------------
#
# Project created by QtCreator 2011-03-19T16:54:18
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_z80_bitset
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_z80_bitset.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../LibCpcEmu/release/ -lLibCpcEmu
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../LibCpcEmu/debug/ -lLibCpcEmu
else:unix: LIBS += -L$$OUT_PWD/../../../LibCpcEmu/ -lLibCpcEmu

INCLUDEPATH += $$PWD/../../../LibCpcEmu \
               $$PWD/../../../LibCpcEmu/kernel
DEPENDPATH += $$PWD/../../../LibCpcEmu
