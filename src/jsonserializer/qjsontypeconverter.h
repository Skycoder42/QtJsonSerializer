#ifndef QJSONTYPECONVERTER_H
#define QJSONTYPECONVERTER_H

#include "QtJsonSerializer/qtjsonserializer_global.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qvariant.h>

class QJsonTypeConverterPrivate;
//! An interface to create custom serializer type converters
class Q_JSONSERIALIZER_EXPORT QJsonTypeConverter
{
public:
	//! Sample values for a priority value
	enum Priority : qint32 {
		ExtremlyLow = -0x00FFFFFF,
		VeryLow = -0x0000FFFF,
		Low = -0x000000FF,
		Standard = 0x00000000,
		High = 0x000000FF,
		VeryHigh = 0x0000FFFF,
		ExtremlyHigh = 0x00FFFFFF
	};

	//! Helper class passed to the type converter by the serializer. Do not implement yourself
	class Q_JSONSERIALIZER_EXPORT SerializationHelper
	{
	public:
		//! Destructor
		virtual ~SerializationHelper();

		//! Returns a property from the serializer
		virtual QVariant getProperty(const char *name) const = 0;

		//! Serialize a subvalue, represented by a meta property
		virtual QJsonValue serializeSubtype(QMetaProperty property, const QVariant &value) const = 0;
		//! Serialize a subvalue, represented by a type id
		virtual QJsonValue serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint = {}) const = 0;
		//! Deserialize a subvalue, represented by a meta property
		virtual QVariant deserializeSubtype(QMetaProperty property, const QJsonValue &value, QObject *parent) const = 0;
		//! Deserialize a subvalue, represented by a type id
		virtual QVariant deserializeSubtype(int propertyType, const QJsonValue &value, QObject *parent, const QByteArray &traceHint = {}) const = 0;
	};

	//! Constructor
	QJsonTypeConverter();
	//! Destructor
	virtual ~QJsonTypeConverter();

	//! Returns the priority of this converter
	int priority() const;
	//! Sets the priority of this converter
	void setPriority(int priority);

	//! Returns true, if this implementation can convert the given type
	virtual bool canConvert(int metaTypeId) const = 0;
	//! Returns a list of json types this implementation can deserialize
	virtual QList<QJsonValue::Type> jsonTypes() const = 0;

	//! Called by the serializer to serializer your given type
	virtual QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const = 0;
	//! Called by the deserializer to serializer your given type
	virtual QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const = 0;

private:
	QScopedPointer<QJsonTypeConverterPrivate> d;
};

#endif // QJSONTYPECONVERTER_H
