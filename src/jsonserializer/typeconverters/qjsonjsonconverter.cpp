#include "qjsonjsonconverter_p.h"
#include "qjsonserializerexception.h"

bool QJsonJsonConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QJsonValue ||
			metaTypeId == QMetaType::QJsonObject ||
			metaTypeId == QMetaType::QJsonArray;
}

QList<QJsonValue::Type> QJsonJsonConverter::jsonTypes() const
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

QJsonValue QJsonJsonConverter::serialize(int propertyType, const QVariant &value) const
{
	if(propertyType == QMetaType::QJsonValue)
		return value.toJsonValue();
	else if(propertyType == QMetaType::QJsonObject)
		return value.toJsonObject();
	else if(propertyType == QMetaType::QJsonArray)
		return value.toJsonArray();
	else {
		Q_UNREACHABLE();
		return QJsonValue();
	}
}

QVariant QJsonJsonConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent) const
{
	Q_UNUSED(parent)

	if(propertyType == QMetaType::QJsonValue)
		return QVariant::fromValue(value);
	else if(propertyType == QMetaType::QJsonArray && value.isArray())
		return QVariant::fromValue(value.toArray());
	if(propertyType == QMetaType::QJsonObject && value.isObject())
		return QVariant::fromValue(value.toObject());
	else
		throw QJsonDeserializationException(QByteArray("Read json type (") +
											QByteArray::number(value.type()) +
											QByteArray(") does not match the target type: ") +
											QMetaType::typeName(propertyType));
}
