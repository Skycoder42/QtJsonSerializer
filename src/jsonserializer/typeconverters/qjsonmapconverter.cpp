#include "qjsonmapconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <QtCore/QJsonObject>

const QRegularExpression QJsonMapConverter::mapTypeRegex(QStringLiteral(R"__(^(?:QMap|QHash)<\s*QString\s*,\s*(.*?)\s*>$)__"));

bool QJsonMapConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QVariantMap ||
		   metaTypeId == QMetaType::QVariantHash ||
		   mapTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QCborTag> QJsonMapConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {NoTag, static_cast<QCborTag>(QCborSerializer::ExplicitMap)};
}

QList<QCborValue::Type> QJsonMapConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Map};
}

QCborValue QJsonMapConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto metaType = getSubtype(propertyType);

	auto cValue = value;
	if (!cValue.convert(QVariant::Map)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to a variant map. Make shure to register map types via QJsonSerializer::registerMapConverters"));
	}

	const auto map = cValue.toMap();
	QCborMap cborMap;
	for (auto it = map.constBegin(); it != map.constEnd(); ++it)
		cborMap.insert(it.key(), helper()->serializeSubtype(metaType, it.value(), it.key().toUtf8()));
	return cborMap;
}

QVariant QJsonMapConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto metaType = getSubtype(propertyType);

	//generate the map
	QVariantMap map;
	const auto cborMap = value.toMap();
	for (auto it = cborMap.constBegin(); it != cborMap.constEnd(); ++it) {
		const auto key = it.key().toString();
		map.insert(key, helper()->deserializeSubtype(metaType, it.value(), parent, key.toUtf8()));
	}
	return map;
}

int QJsonMapConverter::getSubtype(int mapType) const
{
	int metaType = QMetaType::UnknownType;
	auto match = mapTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(mapType)));
	if (match.hasMatch())
		metaType = QMetaType::type(match.captured(1).toUtf8().trimmed());
	return metaType;
}
