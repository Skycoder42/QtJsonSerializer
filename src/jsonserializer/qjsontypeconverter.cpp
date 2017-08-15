#include "qjsontypeconverter.h"

QJsonTypeConverter::~QJsonTypeConverter() = default;

QJsonTypeConverter::SerializationHelper::~SerializationHelper() = default;

QJsonValue QSimpleJsonTypeConverter::serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const
{
	Q_UNUSED(helper);
	return serialize(propertyType, value);
}

QVariant QSimpleJsonTypeConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper);
	return deserialize(propertyType, value, parent);
}
