QT = core jsonserializer

CONFIG += console
CONFIG -= app_bundle

TARGET = Sample

HEADERS += \
	sampleobject.h \
	samplegadget.h

SOURCES += main.cpp \
	sampleobject.cpp \
	samplegadget.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/jsonserializer/$$TARGET
!install_ok: INSTALLS += target
