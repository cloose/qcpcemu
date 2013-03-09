#-------------------------------------------------
#
# Project created by QtCreator 2011-02-11T23:30:01
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qcpcemu
TEMPLATE = app

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibCpcEmu/release/ -lLibCpcEmu
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibCpcEmu/debug/ -lLibCpcEmu
else:unix: LIBS += -L$$OUT_PWD/../LibCpcEmu/ -lLibCpcEmu

INCLUDEPATH += $$PWD/../LibCpcEmu \
               $$PWD/../LibCpcEmu/kernel
DEPENDPATH += $$PWD/../LibCpcEmu
