TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_localeconverter

include(../convlib.pri)

SOURCES += \
	tst_localeconverter.cpp

include(../../testrun.pri)
