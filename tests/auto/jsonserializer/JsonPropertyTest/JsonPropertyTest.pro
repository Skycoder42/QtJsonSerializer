#-------------------------------------------------
#
# Project created by QtCreator 2017-03-30T23:42:23
#
#-------------------------------------------------

QT       += testlib jsonserializer

QT       -= gui

TARGET = tst_jsonproperty
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += tst_jsonproperty.cpp \
    jsongadget.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    jsongadget.h

mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../../lib\''
