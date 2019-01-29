#ifndef QTJSONSERIALIZER_GLOBAL_H
#define QTJSONSERIALIZER_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef QT_STATIC
#  if defined(QT_BUILD_JSONSERIALIZER_LIB)
#    define Q_JSONSERIALIZER_EXPORT Q_DECL_EXPORT
#  else
#    define Q_JSONSERIALIZER_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_JSONSERIALIZER_EXPORT
#endif

//! Method to register all type converters for basic Qt types
Q_JSONSERIALIZER_EXPORT void qtJsonSerializerRegisterTypes();

//! @file qtjsonserializer_global.h The QtJsonSerializer library header file
#endif // QTJSONSERIALIZER_GLOBAL_H
