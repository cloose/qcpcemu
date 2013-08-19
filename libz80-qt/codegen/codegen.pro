#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T08:40:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = z80-opcode-gen
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

OTHER_FILES += \
    opcodes.lst \
    mktables.spec \
    mktables_v2.spec
