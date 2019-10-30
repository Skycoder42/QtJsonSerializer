#include "qjsonmapconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"
#include "qcontainerwriters.h"

#include <QtCore/QJsonObject>

bool QJsonMapConverter::canConvert(int metaTypeId) const
{
	const QVariant tValue{metaTypeId, nullptr};
	return (tValue.canConvert(QMetaType::QVariantMap) ||
			tValue.canConvert(QMetaType::QVariantHash)) &&
		   QAssociativeWriter::canWrite(metaTypeId);
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
	const auto info = QAssociativeWriter::getInfo(propertyType);

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
		cborMap.insert(helper()->serializeSubtype(info.keyType, it.key(), keyStr + ".key"),
					   helper()->serializeSubtype(info.valueType, it.value(), keyStr + ".value"));
	}
	return cborMap;
}

QVariant QJsonMapConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	//generate the map
	QVariant map{propertyType, nullptr};
	auto writer = QAssociativeWriter::getWriter(map);
	if (!writer) {
		throw QJsonDeserializationException(QByteArray("Given type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(" cannot be accessed via QAssociativeWriter - make shure to register it via QJsonSerializerBase::registerMapConverters"));
	}

	// write from cbor into the map
	const auto info = writer->info();
	const auto cborMap = (value.isTag() ? value.taggedValue() : value).toMap();
	for (const auto entry : cborMap) {
		const QByteArray keyStr = "[" + entry.first.toVariant().toString().toUtf8() + "]";
		writer->add(helper()->deserializeSubtype(info.keyType, entry.first, parent, keyStr + ".key"),
					helper()->deserializeSubtype(info.valueType, entry.second, parent, keyStr + ".value"));
	}
	return map;
}
