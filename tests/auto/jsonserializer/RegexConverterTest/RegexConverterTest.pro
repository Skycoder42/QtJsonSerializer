TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_regexconverter

include(../convlib.pri)

SOURCES += \
	tst_regexconverter.cpp

include(../../testrun.pri)
