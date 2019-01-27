TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_multimapconverter

include(../convlib.pri)

SOURCES += \
    tst_multimapconverter.cpp

include(../../testrun.pri)
