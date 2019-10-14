TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_cborserializer

HEADERS += \
	testgadget.h \
	testobject.h

SOURCES += \
	tst_cborserializer.cpp \
	testgadget.cpp \
	testobject.cpp

include(../../testrun.pri)
