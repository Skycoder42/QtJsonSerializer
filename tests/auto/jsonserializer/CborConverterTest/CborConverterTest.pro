TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_cborconverter

include(../convlib.pri)

SOURCES += \
	tst_cborconverter.cpp

include(../../testrun.pri)
