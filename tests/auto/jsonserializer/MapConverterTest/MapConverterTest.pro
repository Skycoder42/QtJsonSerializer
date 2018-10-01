TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_mapconverter

include(../convlib.pri)

SOURCES += \
	tst_mapconverter.cpp

include(../../testrun.pri)
