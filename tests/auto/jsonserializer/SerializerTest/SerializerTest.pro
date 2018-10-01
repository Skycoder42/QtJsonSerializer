TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_serializer

HEADERS += \
	testgadget.h \
	testobject.h

SOURCES += \
	tst_serializer.cpp \
	testgadget.cpp \
	testobject.cpp

include(../../testrun.pri)
