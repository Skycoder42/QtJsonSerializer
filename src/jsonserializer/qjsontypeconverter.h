#ifndef QJSONTYPECONVERTER_H
#define QJSONTYPECONVERTER_H

#include "QtJsonSerializer/qtjsonserializer_global.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qvariant.h>

class Q_JSONSERIALIZER_EXPORT QJsonTypeConverter
{
public:
	class SerializationHelper
	{
	public:
		virtual ~SerializationHelper();

		virtual QJsonValue serializeSubtype(int propertyType, const QVariant &value) const = 0;
		virtual QVariant deserializeSubtype(int propertyType, const QJsonValue &value, QObject *parent) const = 0;
	};

	virtual ~QJsonTypeConverter();

	virtual bool canConvert(int metaTypeId) const = 0;
	virtual QList<QJsonValue::Type> jsonTypes() const = 0;

	virtual QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const = 0;
	virtual QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const = 0;
};

class Q_JSONSERIALIZER_EXPORT QSimpleJsonTypeConverter : public QJsonTypeConverter
{
public:
	virtual QJsonValue serialize(int propertyType, const QVariant &value) const = 0;
	virtual QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent) const = 0;

	// QJsonTypeConverter interface
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const final;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const final;
};

#endif // QJSONTYPECONVERTER_H
