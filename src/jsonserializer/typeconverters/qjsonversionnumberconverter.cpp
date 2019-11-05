#include "qjsonversionnumberconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <QtCore/QVersionNumber>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool QJsonVersionNumberConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == qMetaTypeId<QVersionNumber>();
}

QList<QCborTag> QJsonVersionNumberConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {static_cast<QCborTag>(QCborSerializer::VersionNumber)};
}

QList<QCborValue::Type> QJsonVersionNumberConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array, QCborValue::String};
}

int QJsonVersionNumberConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	if (tag == static_cast<QCborTag>(QCborSerializer::VersionNumber) &&
		allowedCborTypes(QMetaType::UnknownType, tag).contains(dataType))
		return qMetaTypeId<QVersionNumber>();
	else
		return QMetaType::UnknownType;
}

QCborValue QJsonVersionNumberConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	const auto version = value.value<QVersionNumber>();
	if (helper()->getProperty("versionAsString").toBool())
		return {static_cast<QCborTag>(QCborSerializer::VersionNumber), version.toString()};
	else {
		QCborArray array;
		for (const auto segment : version.segments())
			array.append(segment);
		return {static_cast<QCborTag>(QCborSerializer::VersionNumber), array};
	}
}

QVariant QJsonVersionNumberConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	const auto cValue = (value.isTag() ? value.taggedValue() : value);
	if (cValue.type() == QCborValue::Array) {
		const auto cArray = cValue.toArray();
		QVector<int> segments;
		segments.reserve(static_cast<int>(cArray.size()));
		auto i = 0;
		for (const auto cElem : cArray) {
			if (!cElem.isInteger()) {
				throw QJsonDeserializationException{"Segment at position " + QByteArray::number(i) +
													" is not an integer - a version number must be integers only!"};
			}
			segments.append(static_cast<int>(cElem.toInteger()));
			++i;
		}
		return QVariant::fromValue(QVersionNumber{std::move(segments)});
	} else if (cValue.type() == QCborValue::String) {
		const auto strValue = cValue.toString();
		if (!strValue.isEmpty()) {
			int suffixIndex = -1;
			const auto version = QVersionNumber::fromString(strValue, &suffixIndex);
			if (version.isNull())
				throw QJsonDeserializationException("Invalid version number, no segments found");
			if (suffixIndex < strValue.size())
				qWarning() << "Parsed QVersionNumber with suffix - suffixes are discarded!";
			return QVariant::fromValue(version);
		} else
			return QVariant::fromValue(QVersionNumber{});
	} else
		throw QJsonDeserializationException{"Invalid type id"};
}
