#ifndef QJSONSERIALIZER_P_H
#define QJSONSERIALIZER_P_H

#include "qtjsonserializer_global.h"
#include "qjsonserializer.h"

#include <QtCore/QReadWriteLock>
#include <QtCore/QHash>

class Q_JSONSERIALIZER_EXPORT QJsonSerializerPrivate
{
	Q_DISABLE_COPY(QJsonSerializerPrivate)
	friend class QJsonSerializer;

public:
	static QByteArray getTypeName(int propertyType);

	QJsonSerializerPrivate() = default;

	static QReadWriteLock typedefLock;
	static QHash<int, QByteArray> typedefMapping;

	static QReadWriteLock factoryLock;
	static QList<QSharedPointer<QJsonTypeConverterFactory>> typeConverterFactories;

	bool allowNull = false;
	bool keepObjectName = false;
	bool enumAsString = false;
	bool validateBase64 = true;
	bool useBcp47Locale = true;
	QJsonSerializer::ValidationFlags validationFlags = QJsonSerializer::StandardValidation;
	QJsonSerializer::Polymorphing polymorphing = QJsonSerializer::Enabled;
	QJsonSerializer::MultiMapMode multiMapMode = QJsonSerializer::MultiMapMode::Map; //TODO which one is the better default?

	QReadWriteLock typeConverterLock{};
	QList<QSharedPointer<QJsonTypeConverter>> typeConverters;
	QHash<int, QSharedPointer<QJsonTypeConverter>> typeConverterSerCache;
	QHash<int, QSharedPointer<QJsonTypeConverter>> typeConverterDeserCache;

	QSharedPointer<QJsonTypeConverter> findConverter(int propertyType, QJsonValue::Type valueType = QJsonValue::Undefined);
};

#endif // QJSONSERIALIZER_P_H
