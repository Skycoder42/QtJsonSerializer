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

runtarget.target = run-tests
win32: runtarget.depends += $(DESTDIR_TARGET)
else: runtarget.depends += $(TARGET)
QMAKE_EXTRA_TARGETS += runtarget
