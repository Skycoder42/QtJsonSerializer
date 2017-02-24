HEADERS += \
	$$PWD/src/qjsonserializer.h \
	$$PWD/src/qjsonserializerexception.h

SOURCES += \
	$$PWD/src/qjsonserializer.cpp \
	$$PWD/src/qjsonserializerexception.cpp

INCLUDEPATH += $$PWD/src

qjs_as_lib: DEFINES += QJSONSERIALIZER_AS_DLL QJSONSERIALIZER_LIBRARY
