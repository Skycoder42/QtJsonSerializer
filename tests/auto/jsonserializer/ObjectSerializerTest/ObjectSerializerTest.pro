TEMPLATE = app

QT       += core jsonserializer testlib
CONFIG   += console
CONFIG   -= app_bundle

TARGET = tst_objectserializer

SOURCES += \
	testobject.cpp \
	brokentestobject.cpp \
	tst_objectserializer.cpp

HEADERS += \
	testobject.h \
	brokentestobject.h
