TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_jsonconverter

include(../convlib.pri)

SOURCES += \
	tst_jsonconverter.cpp

include(../../testrun.pri)
