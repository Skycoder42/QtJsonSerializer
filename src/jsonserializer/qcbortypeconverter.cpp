#include "qcbortypeconverter.h"
#include <QtCore/QSet>

QList<QCborTag> QCborTypeConverter::allowedCborTags() const
{
	return {};
}

bool QCborTypeConverter::canDeserialize(QCborValue::Type valueType, QCborTag tag) const
{
	if (const auto mTags = allowedCborTags(); !mTags.isEmpty())
		return mTags.contains(tag);
	else
		return cborTypes().contains(valueType);
}

QList<QJsonValue::Type> QCborTypeConverter::jsonTypes() const
{
	QSet<QJsonValue::Type> typeSet;
	const auto cTypes = cborTypes();
	typeSet.reserve(cTypes.size());
	for (auto type : cTypes) {
		for (auto jType : cborToJsonType(type))
			typeSet.insert(jType);
	}
	return typeSet.toList();
}

QJsonValue QCborTypeConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_ASSERT(dynamic_cast<const SerializationHelper*>(helper));
	return serializeCbor(propertyType, value, static_cast<const SerializationHelper*>(helper)).toJsonValue();
}

QVariant QCborTypeConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_ASSERT(dynamic_cast<const SerializationHelper*>(helper));
	return deserializeCbor(propertyType, QCborValue::fromJsonValue(value), parent, static_cast<const SerializationHelper*>(helper));
}

QList<QJsonValue::Type> QCborTypeConverter::cborToJsonType(QCborValue::Type type) const
{
	switch (type) {
	case QCborValue::False:
	case QCborValue::True:
		return {QJsonValue::Bool};
	case QCborValue::Integer:
	case QCborValue::Double:
		return {QJsonValue::Double};
	case QCborValue::ByteArray:
	case QCborValue::String:
	case QCborValue::SimpleType:
	case QCborValue::Url:
	case QCborValue::RegularExpression:
	case QCborValue::Uuid:
		return {QJsonValue::String};
	case QCborValue::Array:
		return {QJsonValue::Array};
	case QCborValue::Map:
		return {QJsonValue::Object};
	case QCborValue::Null:
	case QCborValue::Undefined:
		return {QJsonValue::Null};
	case QCborValue::Tag:
	case QCborValue::Invalid:
	default:
		return {QJsonValue::Undefined};
	case QCborValue::DateTime:
		return {QJsonValue::String, QJsonValue::Double};
	}
}



QCborTypeConverter::SerializationHelper::SerializationHelper() = default;



QCborTypeConverterFactory::QCborTypeConverterFactory() = default;

bool QCborTypeConverterFactory::canDeserialize(QCborValue::Type valueType, QCborTag tag) const
{
	if(!_statusConverter)
		_statusConverter = createConverter();
	return _statusConverter.staticCast<const QCborTypeConverter>()->canDeserialize(valueType, tag);
}

QSharedPointer<QJsonTypeConverter> QCborTypeConverterFactory::createConverter() const
{
	return createCborConverter();
}
