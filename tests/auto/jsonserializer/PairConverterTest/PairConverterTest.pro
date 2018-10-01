TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_pairconverter

include(../convlib.pri)

SOURCES += \
	tst_pairconverter.cpp

include(../../testrun.pri)
