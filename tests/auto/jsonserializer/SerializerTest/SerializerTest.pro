TEMPLATE = app

QT = core gui testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_serializer

HEADERS += \
	testconverter.h

SOURCES += \
	testconverter.cpp \
	tst_serializer.cpp

include(../../testrun.pri)
