#ifndef QTJSONSERIALIZER_GLOBAL_H
#define QTJSONSERIALIZER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STATIC_LIB)
    #define Q_JSONSERIALIZER_EXPORT
#else
    #if defined(QT_BUILD_JSONSERIALIZER_LIB)
        #define Q_JSONSERIALIZER_EXPORT Q_DECL_EXPORT
    #else
        #define Q_JSONSERIALIZER_EXPORT Q_DECL_IMPORT
    #endif
#endif

#if !defined(HAS_NOEXCEPT)
    #if defined(__clang__)
        #if __has_feature(cxx_noexcept)
            #define HAS_NOEXCEPT
        #endif
    #else
        #if defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46 || \
            defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190023026
        #define HAS_NOEXCEPT
        #endif
    #endif

    #ifdef HAS_NOEXCEPT
        #define NOEXCEPT noexcept
    #else
        #define NOEXCEPT
    #endif

#endif

#ifndef _MSC_VER
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

#endif // QTJSONSERIALIZER_GLOBAL_H
