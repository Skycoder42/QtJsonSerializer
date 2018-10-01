TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_objectconverter

include(../convlib.pri)

HEADERS += \
	testobject.h

SOURCES += \
	tst_objectconverter.cpp \
	testobject.cpp

include(../../testrun.pri)
