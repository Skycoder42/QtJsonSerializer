#include "stdtupleconverter_p.h"
#include "exception.h"
#include "cborserializer.h"

#include <QtCore/QCborArray>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool StdTupleConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && extractor->baseType() == "tuple";
}

QList<QCborTag> StdTupleConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {static_cast<QCborTag>(CborSerializer::Tuple)};
}

QList<QCborValue::Type> StdTupleConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array};
}

QCborValue StdTupleConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw SerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(". Make shure to register std::tuple types via QJsonSerializer::registerTupleConverters"));
	}

	const auto metaTypes = extractor->subtypes();
	QCborArray array;
	auto max = metaTypes.size();
	for(auto i = 0; i < max; ++i)
		array.append(helper()->serializeSubtype(metaTypes[i], extractor->extract(value, i), "<" + QByteArray::number(i) + ">"));
	return {static_cast<QCborTag>(CborSerializer::Tuple), array};
}

QVariant StdTupleConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw DeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(". Make shure to register std::tuple types via QJsonSerializer::registerTupleConverters"));
	}

	const auto metaTypes = extractor->subtypes();
	const auto cArray = (value.isTag() ? value.taggedValue() : value).toArray();
	if (cArray.size() != metaTypes.size())
		throw DeserializationException{"Expected array with " + QByteArray::number(metaTypes.size()) +
											" elements, but got " + QByteArray::number(cArray.size()) + " instead"};
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QVariant tuple{propertyType, nullptr};
#else
	QVariant tuple{QMetaType(propertyType), nullptr};
#endif
	auto max = metaTypes.size();
	for(auto i = 0; i < max; ++i)
		extractor->emplace(tuple, helper()->deserializeSubtype(metaTypes[i], cArray[i], parent, "<" + QByteArray::number(i) + ">"), i);
	return tuple;
}
