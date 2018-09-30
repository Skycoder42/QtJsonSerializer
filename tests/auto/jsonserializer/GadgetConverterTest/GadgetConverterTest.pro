TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_gadgetconverter

include(../convlib.pri)

HEADERS += \
	testgadget.h

SOURCES += \
	tst_gadgetconverter.cpp \
	testgadget.cpp

include(../../testrun.pri)
