TEMPLATE = app

QT       += core jsonserializer testlib
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11 strict_c++ c++14 c++1z

TARGET = tst_objectserializer

DEFINES -= QT_ASCII_CAST_WARNINGS

SOURCES += \
	testobject.cpp \
	brokentestobject.cpp \
	tst_objectserializer.cpp \
	testpolyobject.cpp

HEADERS += \
	testobject.h \
	brokentestobject.h \
	testpolyobject.h

mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../../lib\''
