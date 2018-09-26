#ifndef QJSONSERIALIZER_P_H
#define QJSONSERIALIZER_P_H

#include "qtjsonserializer_global.h"
#include "qjsonserializer.h"

class Q_JSONSERIALIZER_EXPORT QJsonSerializerPrivate
{
	Q_DISABLE_COPY(QJsonSerializerPrivate)
public:
	bool allowNull = false;
	bool keepObjectName = false;
	bool enumAsString = false;
	bool validateBase64 = true;
	bool useBcp47Locale = true;
	QJsonSerializer::ValidationFlags validationFlags = QJsonSerializer::StandardValidation;
	QJsonSerializer::Polymorphing polymorphing = QJsonSerializer::Enabled;

	QList<QSharedPointer<QJsonTypeConverter>> typeConverters;
	mutable QHash<int, QSharedPointer<QJsonTypeConverter>> typeConverterTypeCache;

	QJsonSerializerPrivate();
};

#endif // QJSONSERIALIZER_P_H
