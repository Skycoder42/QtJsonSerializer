#include "qjsonjsonconverter_p.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

bool QJsonJsonValueConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QJsonValue;
}

QList<QJsonValue::Type> QJsonJsonValueConverter::jsonTypes() const
{
	return { //All valid types
		QJsonValue::Null,
		QJsonValue::Bool,
		QJsonValue::Double,
		QJsonValue::String,
		QJsonValue::Array,
		QJsonValue::Object
	};
}

QJsonValue QJsonJsonValueConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	return value.toJsonValue();
}

QVariant QJsonJsonValueConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	return QVariant::fromValue(value);
}

bool QJsonJsonObjectConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QJsonObject;
}

QList<QJsonValue::Type> QJsonJsonObjectConverter::jsonTypes() const
{
	return {QJsonValue::Object};
}

QJsonValue QJsonJsonObjectConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	return value.toJsonObject();
}

QVariant QJsonJsonObjectConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	return QVariant::fromValue(value.toObject());
}

bool QJsonJsonArrayConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QJsonArray;
}

QList<QJsonValue::Type> QJsonJsonArrayConverter::jsonTypes() const
{
	return {QJsonValue::Array};
}

QJsonValue QJsonJsonArrayConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	return value.toJsonArray();
}

QVariant QJsonJsonArrayConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	return QVariant::fromValue(value.toArray());
}
