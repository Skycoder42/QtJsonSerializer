#ifndef QJSONEXCEPTIONCONTEXT_P_H
#define QJSONEXCEPTIONCONTEXT_P_H

#include "qtjsonserializer_global.h"
#include "qjsonserializerexception.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QThreadStorage>

namespace QtJsonSerializer {

class Q_JSONSERIALIZER_EXPORT QJsonExceptionContext
{
public:
	QJsonExceptionContext(const QMetaProperty &property);
	QJsonExceptionContext(int propertyType, const QByteArray &hint);
	~QJsonExceptionContext();

	static QJsonSerializationException::PropertyTrace currentContext();

private:
	static QThreadStorage<QJsonSerializationException::PropertyTrace> contextStore;
};

}

#endif // QJSONEXCEPTIONCONTEXT_P_H
