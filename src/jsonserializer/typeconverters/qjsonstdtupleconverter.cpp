#include "qjsonstdtupleconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <QtCore/QCborArray>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool QJsonStdTupleConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && extractor->baseType() == "tuple";
}

QList<QCborTag> QJsonStdTupleConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {static_cast<QCborTag>(QCborSerializer::Tuple)};
}

QList<QCborValue::Type> QJsonStdTupleConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array};
}

QCborValue QJsonStdTupleConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw QJsonSerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(". Make shure to register std::tuple types via QJsonSerializer::registerTupleConverters"));
	}

	const auto metaTypes = extractor->subtypes();
	QCborArray array;
	for(auto i = 0, max = metaTypes.size(); i < max; ++i)
		array.append(helper()->serializeSubtype(metaTypes[i], extractor->extract(value, i), "<" + QByteArray::number(i) + ">"));
	return {static_cast<QCborTag>(QCborSerializer::Tuple), array};
}

QVariant QJsonStdTupleConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw QJsonDeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(". Make shure to register std::tuple types via QJsonSerializer::registerTupleConverters"));
	}

	const auto metaTypes = extractor->subtypes();
	const auto cArray = (value.isTag() ? value.taggedValue() : value).toArray();
	if (cArray.size() != metaTypes.size())
		throw QJsonDeserializationException{"Expected array with " + QByteArray::number(metaTypes.size()) +
											" elements, but got " + QByteArray::number(cArray.size()) + " instead"};

	QVariant tuple{propertyType, nullptr};
	for(auto i = 0, max = metaTypes.size(); i < max; ++i)
		extractor->emplace(tuple, helper()->deserializeSubtype(metaTypes[i], cArray[i], parent, "<" + QByteArray::number(i) + ">"), i);
	return tuple;
}
