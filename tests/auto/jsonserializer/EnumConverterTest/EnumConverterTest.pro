TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_enumconverter

include(../convlib.pri)

SOURCES += \
	testclass.cpp \
	tst_enumconverter.cpp

include(../../testrun.pri)

HEADERS += \
	testclass.h
