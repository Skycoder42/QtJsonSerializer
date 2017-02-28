TARGET = QtJsonSerializer

QT = core

OTHER_FILES += doc/Doxyfile
OTHER_FILES += doc/makedoc.sh
OTHER_FILES += doc/*.dox
OTHER_FILES += doc/snippets/*.cpp

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

win32 {
		QMAKE_TARGET_COMPANY = "Skycoder42"
		QMAKE_TARGET_PRODUCT = "QtJsonSerializer"
		QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
		QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

docTarget.target = doxygen
docTarget.commands = chmod u+x $$PWD/doc/makedoc.sh && $$PWD/doc/makedoc.sh "$$PWD" "$$VERSION" "$$[QT_INSTALL_BINS]" "$$[QT_INSTALL_HEADERS]" "$$[QT_INSTALL_DOCS]"
QMAKE_EXTRA_TARGETS += docTarget

DISTFILES += \
    doc/qjsonserializer.dox
