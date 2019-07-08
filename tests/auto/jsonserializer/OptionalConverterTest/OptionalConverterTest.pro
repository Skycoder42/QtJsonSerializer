TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_optionalconverter

include(../convlib.pri)

SOURCES += \
	tst_optionalconverter.cpp

include(../../testrun.pri)
