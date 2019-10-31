TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_smartpointerconverter

include(../convlib.pri)

SOURCES += \
	testobject.cpp \
	tst_smartpointerconverter.cpp

include(../../testrun.pri)

HEADERS += \
	testobject.h
