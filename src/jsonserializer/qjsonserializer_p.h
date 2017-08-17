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
	QJsonSerializer::Polymorphing polymorphing;

	QHash<QJsonValue::Type, QList<QSharedPointer<QJsonTypeConverter>>> typeConverters;
	mutable QHash<int, QSharedPointer<QJsonTypeConverter>> typeConverterTypeCache;

	QJsonSerializerPrivate();
};

#endif // QJSONSERIALIZER_P_H
