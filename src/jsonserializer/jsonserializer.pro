TARGET = QtJsonSerializer

QT = core

PUBLIC_HEADERS += \
	$$PWD/qjsonserializer_global.h \
	$$PWD/qjsonserializerexception.h \
	$$PWD/qjsonserializer.h

PRIVATE_HEADERS +=

SOURCES += \
	$$PWD/qjsonserializerexception.cpp \
	$$PWD/qjsonserializer.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)
