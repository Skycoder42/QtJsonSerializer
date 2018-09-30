TEMPLATE = app

QT = core jsonserializer testlib jsonserializer-private
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_typeconverter

SOURCES += \
	tst_typeconverter.cpp \
    dummyserializationhelper.cpp \
    opaquedummy.cpp \
    testgadget.cpp \
    testobject.cpp

include(../../testrun.pri)

HEADERS += \
    dummyserializationhelper.h \
    opaquedummy.h \
    testgadget.h \
    testobject.h
