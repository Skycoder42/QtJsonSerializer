TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_legacygeomconverter

include(../convlib.pri)

SOURCES += \
	tst_legacygeomconverter.cpp

include(../../testrun.pri)
