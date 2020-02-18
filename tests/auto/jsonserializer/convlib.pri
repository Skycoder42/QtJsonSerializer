QT *= core testlib jsonserializer-private

INCLUDEPATH += $$PWD/TypeConverterTestLib
DEPENDPATH += $$PWD/TypeConverterTestLib

win32:!win32-g++:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../TypeConverterTestLib/release/ -lTypeConverterTestLib
else:win32:!win32-g++:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../TypeConverterTestLib/debug/ -lTypeConverterTestLib
else:unix: LIBS += -L$$OUT_PWD/../TypeConverterTestLib/ -lTypeConverterTestLib

win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TypeConverterTestLib/release/TypeConverterTestLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TypeConverterTestLib/debug/TypeConverterTestLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../TypeConverterTestLib/libTypeConverterTestLib.a
