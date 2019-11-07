TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_bitarrayconverter

include(../convlib.pri)

SOURCES += \
	tst_bitarrayconverter.cpp

include(../../testrun.pri)
