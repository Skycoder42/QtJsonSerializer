TEMPLATE = app

QT = core jsonserializer testlib
CONFIG   += console
CONFIG   -= app_bundle

TARGET = tst_gadgetserializer

SOURCES += tst_gadgetserializer.cpp \
	testgadget.cpp \
	brokentestgadget.cpp

HEADERS += \
	testgadget.h \
	brokentestgadget.h
