#include "qjsonmultimapconverter_p.h"
#include "qjsonserializerexception.h"
#include "qjsonserializer.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

const QRegularExpression QJsonMultiMapConverter::mapTypeRegex(QStringLiteral(R"__(^(?:QMultiMap|QMultiHash)<\s*QString\s*,\s*(.*?)\s*>$)__"));

bool QJsonMultiMapConverter::canConvert(int metaTypeId) const
{
	return mapTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QJsonValue::Type> QJsonMultiMapConverter::jsonTypes() const
{
	return {QJsonValue::Object, QJsonValue::Array};
}

QJsonValue QJsonMultiMapConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	const auto metaType = getSubtype(propertyType);

	auto cValue = value;
	if(!cValue.convert(QVariant::Map)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to a variant map. Make shure to register map types via QJsonSerializer::registerMapConverters"));
	}
	const auto map = cValue.toMap();

	switch (helper->getProperty("multiMapMode").value<QJsonSerializer::MultiMapMode>()) {
	case QJsonSerializer::MultiMapMode::Map: {
		QJsonObject object;
		for(auto it = map.constBegin(); it != map.constEnd(); ++it) {
			auto vArray = object.value(it.key()).toArray();
			vArray.append(helper->serializeSubtype(metaType, it.value(), it.key().toUtf8()));
			object.insert(it.key(), vArray);
		}
		return object;
	}
	case QJsonSerializer::MultiMapMode::List: {
		QJsonArray array;
		for(auto it = map.constBegin(); it != map.constEnd(); ++it)
			array.append(QJsonArray {it.key(), helper->serializeSubtype(metaType, it.value(), it.key().toUtf8())});
		return array;
	}
	default:
		Q_UNREACHABLE();
		return {};
	}
}

QVariant QJsonMultiMapConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	const auto metaType = getSubtype(propertyType);

	switch (value.type()) {
	case QJsonValue::Object: {
		QVariantMap map;
		const auto object = value.toObject();
		for(auto it = object.constBegin(); it != object.constEnd(); ++it) {
			if(it->isArray()) {
				for(const auto aValue : it->toArray())
					map.insertMulti(it.key(), helper->deserializeSubtype(metaType, aValue, parent, it.key().toUtf8()));
			} else
				map.insertMulti(it.key(), helper->deserializeSubtype(metaType, it.value(), parent, it.key().toUtf8()));
		}
		return map;
	}
	case QJsonValue::Array: {
		QVariantMap map;
		for(const auto aValue : value.toArray()) {
			auto vPair = aValue.toArray();
			if(vPair.size() != 2)
				throw QJsonDeserializationException("Json array must have exactly 2 elements to be read as a value of a multi map");
			map.insertMulti(vPair[0].toString(), helper->deserializeSubtype(metaType, vPair[1], parent, vPair[0].toString().toUtf8()));
		}
		return map;
	}
	default:
		throw QJsonDeserializationException("Unsupported JSON-Type: " + QByteArray::number(value.type()));
	}
}

int QJsonMultiMapConverter::getSubtype(int mapType) const
{
	int metaType = QMetaType::UnknownType;
	auto match = mapTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(mapType)));
	if(match.hasMatch())
		metaType = QMetaType::type(match.captured(1).toUtf8().trimmed());
	return metaType;
}
