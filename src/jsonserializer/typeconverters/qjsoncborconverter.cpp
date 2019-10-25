#include "qjsoncborconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QSet>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

bool QJsonCborConverter::canConvert(int metaTypeId) const
{
	static const QSet<int> metaTypes {
		QMetaType::QCborValue,
		QMetaType::QCborMap,
		QMetaType::QCborArray,
		QMetaType::QCborSimpleType,
		QMetaType::QJsonValue,
		QMetaType::QJsonObject,
		QMetaType::QJsonArray,
		QMetaType::QJsonDocument,
	};
	return metaTypes.contains(metaTypeId);
}

QList<QCborValue::Type> QJsonCborConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(tag)
	switch (metaTypeId) {
	case QMetaType::QCborValue:{
		QList<QCborValue::Type> types;
		const auto metaEnum = QMetaEnum::fromType<QCborValue::Type>();
		types.reserve(metaEnum.keyCount());
		for (auto i = 0; i < metaEnum.keyCount(); ++i) {
			if (const auto value = metaEnum.value(i); value != QCborValue::Invalid)
				types.append(static_cast<QCborValue::Type>(value));
		}
		return types;
	}
	case QMetaType::QJsonValue:
		return {
			QCborValue::Null,
			QCborValue::True,
			QCborValue::False,
			QCborValue::Double,
			QCborValue::String,
			QCborValue::Array,
			QCborValue::Map
		};
	case QMetaType::QCborSimpleType:
		return {
			QCborValue::SimpleType,
			QCborValue::True,
			QCborValue::False,
			QCborValue::Null,
			QCborValue::Undefined
		};
	case QMetaType::QCborMap:
	case QMetaType::QJsonObject:
		return {QCborValue::Map};
	case QMetaType::QCborArray:
	case QMetaType::QJsonArray:
		return {QCborValue::Array};
	case QMetaType::QJsonDocument:
		return {QCborValue::Map, QCborValue::Array, QCborValue::Null};
	default:
		Q_UNREACHABLE();
	}
}

QCborValue QJsonCborConverter::serialize(int propertyType, const QVariant &value) const
{
	switch (propertyType) {
	case QMetaType::QCborValue:
	case QMetaType::QCborSimpleType:
	case QMetaType::QCborMap:
	case QMetaType::QCborArray:
		return value.value<QCborValue>();
	case QMetaType::QJsonValue:
	case QMetaType::QJsonObject:
	case QMetaType::QJsonArray:
	case QMetaType::QJsonDocument:
		return QCborValue::fromJsonValue(value.toJsonValue());
	default:
		throw QJsonSerializationException{"Unsupported type"};
	}
}

QVariant QJsonCborConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(parent)
	switch (propertyType) {
	case QMetaType::QJsonValue:
		return QVariant::fromValue(value.toJsonValue());
	case QMetaType::QJsonObject:
		return QVariant::fromValue(value.toJsonValue().toObject());
	case QMetaType::QJsonArray:
		return QVariant::fromValue(value.toJsonValue().toArray());
	case QMetaType::QJsonDocument: {
		const auto jValue = value.toJsonValue();
		switch (jValue.type()) {
		case QJsonValue::Array:
			return QVariant::fromValue(QJsonDocument{jValue.toArray()});
		case QJsonValue::Object:
			return QVariant::fromValue(QJsonDocument{jValue.toObject()});
		default:
			return QVariant::fromValue(QJsonDocument{});
		}
	}
	case QMetaType::QCborValue:
	case QMetaType::QCborSimpleType:
	case QMetaType::QCborMap:
	case QMetaType::QCborArray:
		return QVariant::fromValue(value);
	default:
		throw QJsonDeserializationException{"Unsupported type"};
	}
}
