#include "pairconverter_p.h"
#include "exception.h"
#include "cborserializer.h"

#include <QtCore/QCborArray>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool PairConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && extractor->baseType() == "pair";
}

QList<QCborTag> PairConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {NoTag, static_cast<QCborTag>(CborSerializer::Pair)};
}

QList<QCborValue::Type> PairConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array};
}

QCborValue PairConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw SerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(". Make shure to register pair types via QJsonSerializer::registerPairConverters"));
	}

	const auto subTypes = extractor->subtypes();
	return {
		static_cast<QCborTag>(CborSerializer::Pair),
		QCborArray {
			helper()->serializeSubtype(subTypes[0], extractor->extract(value, 0), "first"),
			helper()->serializeSubtype(subTypes[1], extractor->extract(value, 1), "second")
		}
	};
}

QVariant PairConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw DeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(". Make shure to register pair types via QJsonSerializer::registerPairConverters"));
	}

	const auto array = (value.isTag() ? value.taggedValue() : value).toArray();
	if (array.size() != 2)
		throw DeserializationException("CBOR/JSON array must have exactly 2 elements to be read as a pair");

	const auto subTypes = extractor->subtypes();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QVariant resPair{propertyType, nullptr};
#else
	QVariant resPair{QMetaType(propertyType), nullptr};
#endif
	extractor->emplace(resPair, helper()->deserializeSubtype(subTypes[0], array[0], parent, "first"), 0);
	extractor->emplace(resPair, helper()->deserializeSubtype(subTypes[1], array[1], parent, "second"), 1);
	return resPair;
}
