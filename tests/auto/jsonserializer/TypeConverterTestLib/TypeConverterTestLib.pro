TEMPLATE = lib

QT = core testlib jsonserializer jsonserializer-private
CONFIG += staticlib

TARGET = TypeConverterTestLib

HEADERS += \
	typeconvertertestbase.h \
	dummyserializationhelper.h \
	opaquedummy.h \
    multitypeconvertertestbase.h

SOURCES += \
	typeconvertertestbase.cpp \
	dummyserializationhelper.cpp \
	opaquedummy.cpp \
    multitypeconvertertestbase.cpp

QMAKE_EXTRA_TARGETS += run-tests
