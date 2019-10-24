#include "qjsonlistconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <QtCore/QJsonArray>

const QRegularExpression QJsonListConverter::listTypeRegex(QStringLiteral(R"__(^(QList|QLinkedList|QVector|QStack|QQueue|QSet)<\s*(.*?)\s*>$)__"));

bool QJsonListConverter::canConvert(int metaTypeId) const
{
	// NOTE with writeable iterators, only the `value.canConvert(QMetaType::QVariantList)` would be needed
	return metaTypeId == QMetaType::QVariantList ||
			metaTypeId == QMetaType::QStringList ||
			metaTypeId == QMetaType::QByteArrayList ||
			listTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QCborTag> QJsonListConverter::allowedCborTags(int metaTypeId) const
{
	const auto isSet = getSubtype(metaTypeId).second;
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
	const auto [metaType, isSet] = getSubtype(propertyType);

	if (!value.canConvert(QMetaType::QVariantList)) {
		throw QJsonSerializationException(QByteArray("Given type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" cannot be processed via QSequentialIterable - make shure to register the container type via Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE"));
	}

	QCborArray array;
	auto index = 0;
	for (const auto &element : value.value<QSequentialIterable>())
		array.append(helper()->serializeSubtype(metaType, element, "[" + QByteArray::number(index++) + "]"));
	if (isSet)
		return {static_cast<QCborTag>(QCborSerializer::Set), array};
	else
		return array;
}

QVariant QJsonListConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto metaType = getSubtype(propertyType).first;

	//generate the list
	QVariant list{propertyType, nullptr};
	auto writer = QSequentialWriter::getWriter(list);
	if (!writer.isValid()) {
		throw QJsonSerializationException(QByteArray("Given type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" cannot be accessed via QSequentialWriter - make shure to register it via QJsonSerializerBase::registerListConverters or QJsonSerializerBase::registerSetConverters"));
	}

	const auto array = (value.isTag() ? value.taggedValue() : value).toArray();
	auto index = 0;
	writer.reserve(static_cast<int>(array.size()));
	for (auto element : (value.isTag() ? value.taggedValue() : value).toArray())
		writer.add(helper()->deserializeSubtype(metaType, element, parent, "[" + QByteArray::number(index++) + "]"));
	return list;
}

std::pair<int, bool> QJsonListConverter::getSubtype(int listType) const
{
	int metaType = QMetaType::UnknownType;
	auto isSet = false;
	if (listType == QMetaType::QStringList)
		metaType = QMetaType::QString;
	else if (listType == QMetaType::QByteArrayList)
		metaType = QMetaType::QByteArray;
	else if (listType != QMetaType::QVariantList) {
		auto match = listTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(listType)));
		if (match.hasMatch()) {
			isSet = match.captured(1) == QStringLiteral("QSet");
			metaType = QMetaType::type(match.captured(2).toUtf8().trimmed());
		}
	}

	return std::make_pair(metaType, isSet);
}
