TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_chronodurationconverter

include(../convlib.pri)

SOURCES += \
	tst_chronodurationconverter.cpp

include(../../testrun.pri)
