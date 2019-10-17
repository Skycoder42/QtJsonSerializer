TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_serializer

HEADERS += \
	testconverter.h \
	testgadget.h \
	testobject.h

SOURCES += \
	testconverter.cpp \
	tst_serializer.cpp \
	testgadget.cpp \
	testobject.cpp

include(../../testrun.pri)
