TEMPLATE = lib

QT = core testlib jsonserializer jsonserializer-private
CONFIG += staticlib

TARGET = TypeConverterTestLib

HEADERS += \
	typeconvertertestbase.h \
	dummyserializationhelper.h \
	opaquedummy.h

SOURCES += \
	typeconvertertestbase.cpp \
	dummyserializationhelper.cpp \
	opaquedummy.cpp

QMAKE_EXTRA_TARGETS += run-tests
