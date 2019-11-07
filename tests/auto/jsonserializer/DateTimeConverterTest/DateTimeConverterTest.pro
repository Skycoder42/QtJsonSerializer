TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_datetimeconverter

include(../convlib.pri)

SOURCES += \
	tst_datetimeconverter.cpp

include(../../testrun.pri)
