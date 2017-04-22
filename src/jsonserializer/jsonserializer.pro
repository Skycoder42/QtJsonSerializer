TARGET = QtJsonSerializer

QT = core

SOURCES += \
	qjsonserializerexception.cpp \
	qjsonserializer.cpp

HEADERS += \
	qjsonserializerexception.h \
	qjsonserializer.h \
    qjsonserializer_p.h \
    qtjsonserializer_global.h \
    qjsonserializer_helpertypes.h

load(qt_module)

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "QtJsonSerializer"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
