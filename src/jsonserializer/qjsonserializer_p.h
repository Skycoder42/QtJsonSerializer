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

	QMultiHash<QJsonValue::Type, QSharedPointer<QJsonTypeConverter>> typeConverters;
	mutable QHash<int, QSharedPointer<QJsonTypeConverter>> typeConverterTypeCache;

	QJsonSerializerPrivate();

	static QJsonSerializerPrivate *fromHelper(const QJsonTypeConverter::SerializationHelper *helper);//TODO use "get property" instead
};

#endif // QJSONSERIALIZER_P_H
