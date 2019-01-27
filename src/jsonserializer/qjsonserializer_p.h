#ifndef QJSONSERIALIZER_P_H
#define QJSONSERIALIZER_P_H

#include "qtjsonserializer_global.h"
#include "qjsonserializer.h"

#include <QtCore/QReadWriteLock>
#include <QtCore/QHash>

class Q_JSONSERIALIZER_EXPORT QJsonSerializerPrivate
{
	Q_DISABLE_COPY(QJsonSerializerPrivate)
public:
	static QByteArray getTypeName(int propertyType);

	QJsonSerializerPrivate();

private:
	friend class QJsonSerializer;

	static QReadWriteLock typedefLock;
	static QHash<int, QByteArray> typedefMapping;

	bool allowNull = false;
	bool keepObjectName = false;
	bool enumAsString = false;
	bool validateBase64 = true;
	bool useBcp47Locale = true;
	QJsonSerializer::ValidationFlags validationFlags = QJsonSerializer::StandardValidation;
	QJsonSerializer::Polymorphing polymorphing = QJsonSerializer::Enabled;
	QJsonSerializer::MultiMapMode multiMapMode = QJsonSerializer::MultiMapMode::Map; //TODO which one is the better default?

	QList<QSharedPointer<QJsonTypeConverter>> typeConverters;
	mutable QHash<int, QSharedPointer<QJsonTypeConverter>> typeConverterTypeCache;
};

#endif // QJSONSERIALIZER_P_H
