TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_versionconverter

include(../convlib.pri)

SOURCES += \
	tst_versionconverter.cpp

include(../../testrun.pri)
