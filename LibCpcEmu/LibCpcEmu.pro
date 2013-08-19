#-------------------------------------------------
#
# Project created by QtCreator 2011-01-30T13:28:19
#
#-------------------------------------------------

#QT       -= gui

TARGET = LibCpcEmu
TEMPLATE = lib

DEFINES += LIBCPCEMU_LIBRARY
HEADERS += LibCpcEmu_global.h \
    exceptions.h \
    cpcsystem.h

SOURCES += \
    cpcsystem.cpp \
    cpcsystem_p.cpp

include(kernel/kernel.pri)
include(utils/utils.pri)

INCLUDEPATH += $$PWD\kernel

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libz80-qt/src/release/ -llibz80-qt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libz80-qt/src/debug/ -llibz80-qt
else:unix: LIBS += -L$$OUT_PWD/../libz80-qt/src/ -llibz80-qt

INCLUDEPATH += $$PWD/../libz80-qt/src
DEPENDPATH += $$PWD/../libz80-qt/src
