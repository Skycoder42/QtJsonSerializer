#ifndef QTJSONSERIALIZER_EXCEPTION_P_H
#define QTJSONSERIALIZER_EXCEPTION_P_H

#include "qtjsonserializer_global.h"
#include "exception.h"

namespace QtJsonSerializer {

class Q_JSONSERIALIZER_EXPORT ExceptionPrivate
{
	Q_DISABLE_COPY(ExceptionPrivate)
public:
	ExceptionPrivate(QByteArray message);

	QByteArray message;
	Exception::PropertyTrace trace;
	QByteArray what;
};

}

#endif // QTJSONSERIALIZER_EXCEPTION_P_H
