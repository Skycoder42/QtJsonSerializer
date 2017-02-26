TARGET = QtJsonSerializer

QT = core

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = $$TARGET
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

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
