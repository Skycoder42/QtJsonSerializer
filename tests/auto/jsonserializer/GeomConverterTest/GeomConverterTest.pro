TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_geomconverter

include(../convlib.pri)

SOURCES += \
	tst_geomconverter.cpp

include(../../testrun.pri)
