TARGET = QtJsonSerializer

QT = core core-private

HEADERS += \
	qcborserializer.h \
	qcborserializer_p.h \
	qcontainerwriters.h \
	qcontainerwriters_p.h \
	qjsonserializer.h \
	qjsonserializer_p.h \
	qjsonserializerbase.h \
	qjsonserializerbase_p.h \
	qjsonserializerexception.h \
	qjsontypeextractors.h \
	qtjsonserializer_global.h \
	qjsonserializer_helpertypes.h \
	qjsontypeconverter.h \
	qjsonexceptioncontext_p.h \
	qjsonserializerexception_p.h

SOURCES += \
	qcborserializer.cpp \
	qcontainerwriters.cpp \
	qjsonserializer.cpp \
	qjsonserializerbase.cpp \
	qjsonserializerexception.cpp \
	qjsontypeconverter.cpp \
	qjsonexceptioncontext.cpp

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
