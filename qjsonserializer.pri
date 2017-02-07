HEADERS += \
	$$PWD/src/QJsonSerializer \
	$$PWD/src/serializerexception.h \
	$$PWD/src/qjsonserializer.h

SOURCES += \
	$$PWD/src/serializerexception.cpp \
	$$PWD/src/qjsonserializer.cpp

INCLUDEPATH += $$PWD/src

qjs_as_lib: DEFINES += QJSONSERIALIZER_AS_DLL QJSONSERIALIZER_LIBRARY
