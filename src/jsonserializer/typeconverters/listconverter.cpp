#include "listconverter_p.h"
#include "exception.h"
#include "cborserializer.h"
#include "metawriters.h"

#include <QtCore/QJsonArray>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;
using namespace QtJsonSerializer::MetaWriters;

bool ListConverter::canConvert(int metaTypeId) const
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	return QVariant{metaTypeId, nullptr}.canConvert(QMetaType::QVariantList) &&
		   SequentialWriter::canWrite(metaTypeId);
#else
	return QVariant{QMetaType(metaTypeId), nullptr}.canConvert(QMetaType(QMetaType::QVariantList)) &&
		   SequentialWriter::canWrite(metaTypeId);
#endif
}

QList<QCborTag> ListConverter::allowedCborTags(int metaTypeId) const
{
	const auto isSet = SequentialWriter::getInfo(metaTypeId).isSet;
	QList<QCborTag> tags {
		NoTag,
		static_cast<QCborTag>(CborSerializer::Homogeneous)
	};
	if (isSet)
		tags.append(static_cast<QCborTag>(CborSerializer::Set));
	return tags;
}

QList<QCborValue::Type> ListConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array};
}

QCborValue ListConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto info = SequentialWriter::getInfo(propertyType);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	if (!value.canConvert(QMetaType::QVariantList)) {
#else
	if (!value.canConvert(QMetaType(QMetaType::QVariantList))) {
#endif
		throw SerializationException(QByteArray("Given type ") +
										  QMetaTypeName(propertyType) +
										  QByteArray(" cannot be processed via QSequentialIterable - make shure to register the container type via Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE"));
	}

	QCborArray array;
	auto index = 0;
	for (const auto &element : value.value<QSequentialIterable>())
		array.append(helper()->serializeSubtype(info.type, element, "[" + QByteArray::number(index++) + "]"));
	if (info.isSet)
		return {static_cast<QCborTag>(CborSerializer::Set), array};
	else
		return array;
}

QVariant ListConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	//generate the list
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QVariant list{propertyType, nullptr};
#else
	QVariant list{QMetaType(propertyType), nullptr};
#endif
	auto writer = SequentialWriter::getWriter(list);
	if (!writer) {
		throw DeserializationException(QByteArray("Given type ") +
											QMetaTypeName(propertyType) +
											QByteArray(" cannot be accessed via QSequentialWriter - make shure to register it via QJsonSerializerBase::registerListConverters or QJsonSerializerBase::registerSetConverters"));
	}

	const auto info = writer->info();
	const auto array = (value.isTag() ? value.taggedValue() : value).toArray();
	auto index = 0;
	writer->reserve(static_cast<int>(array.size()));
	for (auto element : array)
		writer->add(helper()->deserializeSubtype(info.type, element, parent, "[" + QByteArray::number(index++) + "]"));
	return list;
}
