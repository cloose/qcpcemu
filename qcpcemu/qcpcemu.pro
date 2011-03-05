#-------------------------------------------------
#
# Project created by QtCreator 2011-02-11T23:30:01
#
#-------------------------------------------------

QT       += core gui

TARGET = qcpcemu
TEMPLATE = app

INCLUDEPATH += ..\LibCpcEmu \
               ..\LibCpcEmu\kernel

SOURCES += main.cpp\
        mainwindow.cpp \
    debugform.cpp \
    screenwidget.cpp \
    qimagescreenrenderer.cpp

HEADERS  += mainwindow.h \
    debugform.h \
    screenwidget.h \
    qimagescreenrenderer.h

FORMS    += mainwindow.ui \
    debugform.ui

win32:LIBS += ../LibCpcEmu-build-desktop/debug/LibCpcEmu.dll
