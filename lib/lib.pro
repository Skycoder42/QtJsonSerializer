#-------------------------------------------------
#
# Project created by QtCreator 2017-02-07T18:42:11
#
#-------------------------------------------------
TEMPLATE = lib
QT       -= gui

TARGET = Qt5JsonSerializer
VERSION = 1.1.0

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "QtJsonSerializer"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	CONFIG += skip_target_version_ext
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
	QMAKE_FRAMEWORK_BUNDLE_NAME = "QtJsonSerializer"

	CONFIG += lib_bundle
	QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
	QMAKE_LFLAGS += '-Wl,-rpath,\'@executable_path/../Frameworks\''
} else:unix {
	QMAKE_LFLAGS += '-Wl,-rpath,\'.\''
	QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
}

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += qjs_as_lib
include(../qjsonserializer.pri)

DISTFILES += \
	qt_lib_jsonserializer.pri \
	make_module.sh

target.path = /lib
INSTALLS += target
