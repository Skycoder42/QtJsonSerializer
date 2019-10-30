#include "qjsonlistconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"
#include "qcontainerwriters.h"

#include <QtCore/QJsonArray>

bool QJsonListConverter::canConvert(int metaTypeId) const
{
	return QVariant{metaTypeId, nullptr}.canConvert(QMetaType::QVariantList) &&
		   QSequentialWriter::canWrite(metaTypeId);
}

QList<QCborTag> QJsonListConverter::allowedCborTags(int metaTypeId) const
{
	const auto isSet = QSequentialWriter::getInfo(metaTypeId).isSet;
	QList<QCborTag> tags {
		NoTag,
		static_cast<QCborTag>(QCborSerializer::Homogeneous)
	};
	if (isSet)
		tags.append(static_cast<QCborTag>(QCborSerializer::Set));
	return tags;
}

QList<QCborValue::Type> QJsonListConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array};
}

QCborValue QJsonListConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto info = QSequentialWriter::getInfo(propertyType);

	if (!value.canConvert(QMetaType::QVariantList)) {
		throw QJsonSerializationException(QByteArray("Given type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" cannot be processed via QSequentialIterable - make shure to register the container type via Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE"));
	}

	QCborArray array;
	auto index = 0;
	for (const auto &element : value.value<QSequentialIterable>())
		array.append(helper()->serializeSubtype(info.type, element, "[" + QByteArray::number(index++) + "]"));
	if (info.isSet)
		return {static_cast<QCborTag>(QCborSerializer::Set), array};
	else
		return array;
}

QVariant QJsonListConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	//generate the list
	QVariant list{propertyType, nullptr};
	auto writer = QSequentialWriter::getWriter(list);
	if (!writer) {
		throw QJsonDeserializationException(QByteArray("Given type ") +
											QMetaType::typeName(propertyType) +
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
