QT = core jsonserializer

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = Sample

HEADERS += \
	sampleobject.h \
	samplegadget.h

SOURCES += main.cpp \
	sampleobject.cpp \
	samplegadget.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/jsonserializer/$$TARGET
INSTALLS += target

qt_module_build:mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../lib\''
