#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T15:47:28
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = libz80-qt
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = lib


SOURCES += alu.cpp \
    opcodes_table.cpp \
    opcodes_impl.cpp \
    registers.cpp \
    cpu.cpp

HEADERS += \
    alu.h \
    opcodes_decl.h \
    types.h \
    opcodes_table.h \
    registers.h \
    imemory.h \
    cpu.h \
    ioport.h \
    iinputoutput.h
