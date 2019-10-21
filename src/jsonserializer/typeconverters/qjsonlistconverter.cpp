#include "qjsonlistconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <QtCore/QJsonArray>

const QRegularExpression QJsonListConverter::listTypeRegex(QStringLiteral(R"__(^(QList|QLinkedList|QVector|QStack|QQueue|QSet)<\s*(.*?)\s*>$)__"));

bool QJsonListConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QVariantList ||
			metaTypeId == QMetaType::QStringList ||
			listTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QCborTag> QJsonListConverter::allowedCborTags(int metaTypeId) const
{
	auto isSet = false;
	getSubtype(metaTypeId, isSet);
	QList<QCborTag> tags {static_cast<QCborTag>(QCborSerializer::Homogeneous)};
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
	auto isSet = false;
	const auto metaType = getSubtype(propertyType, isSet);

	auto cValue = value;
	if (!cValue.convert(QVariant::List)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to a variant list. Make shure to register list types via QJsonSerializer::registerListConverters (or QJsonSerializer::registerSetConverters)"));
	}

	QCborArray array;
	auto index = 0;
	for (const auto &element : cValue.toList())
		array.append(helper()->serializeSubtype(metaType, element, "[" + QByteArray::number(index++) + "]"));
	return {static_cast<QCborTag>(isSet ? QCborSerializer::Set : QCborSerializer::Homogeneous), array};
}

QVariant QJsonListConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	auto isSet = false;
	const auto metaType = getSubtype(propertyType, isSet);

	//generate the list
	QVariantList list;
	auto index = 0;
	for (auto element : (value.isTag() ? value.taggedValue() : value).toArray())
		list.append(helper()->deserializeSubtype(metaType, element, parent, "[" + QByteArray::number(index++) + "]"));
	return list;
}

int QJsonListConverter::getSubtype(int listType, bool &isSet) const
{
	int metaType = QMetaType::UnknownType;
	if (listType == QMetaType::QStringList)
		metaType = QMetaType::QString;
	else if (listType != QMetaType::QVariantList) {
		auto match = listTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(listType)));
		if (match.hasMatch()) {
			isSet = match.captured(1) == QStringLiteral("QSet");
			metaType = QMetaType::type(match.captured(2).toUtf8().trimmed());
		}
	}

	return metaType;
}
