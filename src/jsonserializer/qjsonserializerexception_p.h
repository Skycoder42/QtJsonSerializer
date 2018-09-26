#ifndef QJSONSERIALIZEREXCEPTION_P_H
#define QJSONSERIALIZEREXCEPTION_P_H

#include "qtjsonserializer_global.h"
#include "qjsonserializerexception.h"

class Q_JSONSERIALIZER_EXPORT QJsonSerializationExceptionPrivate
{
	Q_DISABLE_COPY(QJsonSerializationExceptionPrivate)
public:
	QJsonSerializationExceptionPrivate(QByteArray message);

	QByteArray message;
	QJsonSerializerException::PropertyTrace trace;
	QByteArray what;
};

#endif // QJSONSERIALIZEREXCEPTION_P_H
