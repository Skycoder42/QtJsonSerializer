TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_listconverter

include(../convlib.pri)

SOURCES += \
	tst_listconverter.cpp

include(../../testrun.pri)
