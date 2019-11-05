TARGET = QtJsonSerializer

QT = core core-private

HEADERS += \
	cborserializer.h \
	cborserializer_p.h \
	exception.h \
	exception_p.h \
	exceptioncontext_p.h \
	jsonserializer.h \
	jsonserializer_p.h \
	metawriters.h \
	metawriters_p.h \
	qtjsonserializer_global.h \
	qtjsonserializer_helpertypes.h \
	serializerbase.h \
	serializerbase_p.h \
	typeconverter.h \
	typeextractors.h

SOURCES += \
	cborserializer.cpp \
	exception.cpp \
	exceptioncontext.cpp \
	jsonserializer.cpp \
	metawriters.cpp \
	serializerbase.cpp \
	typeconverter.cpp

include(typeconverters/typeconverters.pri)

no_register_json_converters: DEFINES += NO_REGISTER_JSON_CONVERTERS
else: include(qjsonreggen.pri)

load(qt_module)

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "QtJsonSerializer"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

DISTFILES += \
	typesplit.pri
