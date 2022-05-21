#include "mapconverter_p.h"
#include "exception.h"
#include "cborserializer.h"
#include "metawriters.h"

#include <QtCore/QJsonObject>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;
using namespace QtJsonSerializer::MetaWriters;

bool MapConverter::canConvert(int metaTypeId) const
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	const QVariant tValue{metaTypeId, nullptr};
	return (tValue.canConvert(QMetaType::QVariantMap) ||
			tValue.canConvert(QMetaType::QVariantHash)) &&
		   AssociativeWriter::canWrite(metaTypeId);

#else
	const QVariant tValue{QMetaType(metaTypeId), nullptr};
	return (tValue.canConvert(QMetaType(QMetaType::QVariantMap)) ||
			tValue.canConvert(QMetaType(QMetaType::QVariantHash))) &&
		   AssociativeWriter::canWrite(metaTypeId);

#endif
}

QList<QCborTag> MapConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {NoTag, static_cast<QCborTag>(CborSerializer::ExplicitMap)};
}

QList<QCborValue::Type> MapConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Map};
}

QCborValue MapConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto info = AssociativeWriter::getInfo(propertyType);

	// verify is readable
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	if (!value.canConvert(QMetaType::QVariantMap) &&
		!value.canConvert(QMetaType::QVariantHash)) {
#else
	if (!value.canConvert(QMetaType(QMetaType::QVariantMap)) &&
		!value.canConvert(QMetaType(QMetaType::QVariantHash))) {
#endif
		throw SerializationException(QByteArray("Given type ") +
										  QMetaTypeName(propertyType) +
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

QVariant MapConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	//generate the map
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QVariant map{propertyType, nullptr};
#else
	QVariant map{QMetaType(propertyType), nullptr};
#endif
	auto writer = AssociativeWriter::getWriter(map);
	if (!writer) {
		throw DeserializationException(QByteArray("Given type ") +
											QMetaTypeName(propertyType) +
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
