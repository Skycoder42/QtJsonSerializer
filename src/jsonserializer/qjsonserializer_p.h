#ifndef QJSONSERIALIZER_P_H
#define QJSONSERIALIZER_P_H

#include "qtjsonserializer_global.h"
#include "qjsonserializer.h"

class Q_JSONSERIALIZER_EXPORT QJsonSerializerPrivate
{
public:
	bool allowNull;
	bool keepObjectName;
	bool enumAsString;
	QJsonSerializer::ValidationFlags validationFlags;

	QJsonSerializerPrivate();
};

#endif // QJSONSERIALIZER_P_H
