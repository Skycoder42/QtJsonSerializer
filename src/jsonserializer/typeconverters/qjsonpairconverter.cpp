#include "qjsonpairconverter_p.h"
#include "qjsonserializerexception.h"
#include <QCborSerializer>

#include <QtCore/QCborArray>

bool QJsonPairConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && extractor->baseType() == "pair";
}

QList<QCborTag> QJsonPairConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {NoTag, static_cast<QCborTag>(QCborSerializer::Pair)};
}

QList<QCborValue::Type> QJsonPairConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array};
}

QCborValue QJsonPairConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw QJsonSerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(". Make shure to register pair types via QJsonSerializer::registerPairConverters"));
	}

	const auto subTypes = extractor->subtypes();
	return {
		static_cast<QCborTag>(QCborSerializer::Pair),
		QCborArray {
			helper()->serializeSubtype(subTypes[0], extractor->extract(value, 0), "first"),
			helper()->serializeSubtype(subTypes[1], extractor->extract(value, 1), "second")
		}
	};
}

QVariant QJsonPairConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw QJsonDeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(". Make shure to register pair types via QJsonSerializer::registerPairConverters"));
	}

	const auto array = (value.isTag() ? value.taggedValue() : value).toArray();
	if (array.size() != 2)
		throw QJsonDeserializationException("CBOR/JSON array must have exactly 2 elements to be read as a pair");

	const auto subTypes = extractor->subtypes();
	QVariant resPair{propertyType, nullptr};
	extractor->emplace(resPair, helper()->deserializeSubtype(subTypes[0], array[0], parent, "first"), 0);
	extractor->emplace(resPair, helper()->deserializeSubtype(subTypes[1], array[1], parent, "second"), 1);
	return resPair;
}
