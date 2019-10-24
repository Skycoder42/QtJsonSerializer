#include "qjsonmapconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <QtCore/QJsonObject>

const QRegularExpression QJsonMapConverter::mapTypeRegex(QStringLiteral(R"__(^(?:QMap|QHash)<(.*?)>$)__"));

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
	const auto [keyType, valueType] = getSubtype(propertyType);

	// verify is readable
	if (!value.canConvert(QMetaType::QVariantMap) &&
		!value.canConvert(QMetaType::QVariantHash)) {
		throw QJsonSerializationException(QByteArray("Given type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" cannot be processed via QAssociativeIterable - make shure to register the container type via Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE"));
	}

	// write from map to cbor
	const auto iterable = value.value<QAssociativeIterable>();
	QCborMap cborMap;
	for (auto it = iterable.begin(), end = iterable.end(); it != end; ++it) {
		const QByteArray keyStr = "[" + it.key().toString().toUtf8() + "]";
		cborMap.insert(helper()->serializeSubtype(keyType, it.key(), keyStr + ".key"),
					   helper()->serializeSubtype(valueType, it.value(), keyStr + ".value"));
	}
	return cborMap;
}

QVariant QJsonMapConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto [keyType, valueType] = getSubtype(propertyType);

	//generate the map
	QVariant map{propertyType, nullptr};
	auto writer = QAssociativeWriter::getWriter(map);
	if (!writer.isValid()) {
		throw QJsonDeserializationException(QByteArray("Given type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(" cannot be accessed via QAssociativeWriter - make shure to register it via QJsonSerializerBase::registerMapConverters"));
	}

	// write from cbor into the map
	const auto cborMap = (value.isTag() ? value.taggedValue() : value).toMap();
	for (const auto entry : cborMap) {
		const QByteArray keyStr = "[" + entry.first.toVariant().toString().toUtf8() + "]";
		writer.add(helper()->deserializeSubtype(keyType, entry.first, parent, keyStr + ".key"),
				   helper()->deserializeSubtype(valueType, entry.second, parent, keyStr + ".value"));
	}
	return map;
}

std::pair<int, int> QJsonMapConverter::getSubtype(int mapType) const
{
	if (mapType == QMetaType::QVariantMap ||
		mapType == QMetaType::QVariantHash)
		return std::make_pair(QMetaType::QString, QMetaType::QVariant);

	int keyType = QMetaType::UnknownType;
	int valueType = QMetaType::UnknownType;
	auto match = mapTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(mapType)));
	if (match.hasMatch()) {
		// parse match, using <> and , rules
		const auto matchStr = match.captured(1);
		auto bCount = 0;
		for (auto i = 0; i < matchStr.size(); ++i) {
			const auto &c = matchStr[i];
			if (c == QLatin1Char('<'))
				++bCount;
			else if (c == QLatin1Char('>'))
				--bCount;
			else if (bCount == 0 && c == QLatin1Char(',')) {
				keyType = QMetaType::type(matchStr.mid(0, i).trimmed().toUtf8());
				valueType = QMetaType::type(matchStr.mid(i + 1).trimmed().toUtf8());
				break;
			}
		}
	}

	return std::make_pair(keyType, valueType);
}
