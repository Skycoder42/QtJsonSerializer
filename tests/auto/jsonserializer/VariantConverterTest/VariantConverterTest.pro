TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_variantconverter

include(../convlib.pri)

SOURCES += \
	tst_variantconverter.cpp

include(../../testrun.pri)
