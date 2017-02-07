#-------------------------------------------------
#
# Project created by QtCreator 2017-02-07T18:42:11
#
#-------------------------------------------------
TEMPLATE = lib
QT       -= gui

TARGET = QJsonSerializer
VERSION = 1.0.0

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += qjs_as_lib
include(../qjsonserializer.pri)

unix {
	target.path = /usr/lib
	INSTALLS += target
}
