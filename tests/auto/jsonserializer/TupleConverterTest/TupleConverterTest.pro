TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_tupleconverter

include(../convlib.pri)

SOURCES += \
	tst_tupleconverter.cpp

include(../../testrun.pri)
