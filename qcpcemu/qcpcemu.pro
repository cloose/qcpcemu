#-------------------------------------------------
#
# Project created by QtCreator 2011-02-11T23:30:01
#
#-------------------------------------------------

QT       += core gui

TARGET = qcpcemu
TEMPLATE = app

INCLUDEPATH += ..\LibCpcEmu

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:LIBS += ../LibCpcEmu-build-desktop/debug/LibCpcEmu.dll
