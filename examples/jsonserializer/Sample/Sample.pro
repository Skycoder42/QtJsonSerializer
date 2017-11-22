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

#not found by linker?
unix:!mac {
	LIBS += -L$$OUT_PWD/../../../lib #required to make this the first place to search
	LIBS += -L$$[QT_INSTALL_LIBS] -licudata
	LIBS += -L$$[QT_INSTALL_LIBS] -licui18n
	LIBS += -L$$[QT_INSTALL_LIBS] -licuuc
}

qt_module_build:mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../lib\''
