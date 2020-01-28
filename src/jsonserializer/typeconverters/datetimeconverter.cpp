#include "datetimeconverter_p.h"
#include "exception.h"
#include "cborserializer.h"
#include <QtCore/QSet>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool DateTimeConverter::canConvert(int metaTypeId) const
{
	static const QSet<int> types {
		QMetaType::QDateTime,
		QMetaType::QDate,
		QMetaType::QTime
	};
	return types.contains(metaTypeId);
}

QList<QCborTag> DateTimeConverter::allowedCborTags(int metaTypeId) const
{
	switch (metaTypeId) {
	case QMetaType::QDateTime:
		return {static_cast<QCborTag>(QCborKnownTags::DateTimeString), static_cast<QCborTag>(QCborKnownTags::UnixTime_t)};
	case QMetaType::QDate:
		return {static_cast<QCborTag>(QCborKnownTags::DateTimeString), static_cast<QCborTag>(CborSerializer::Date)};
	case QMetaType::QTime:
		return {static_cast<QCborTag>(QCborKnownTags::DateTimeString), static_cast<QCborTag>(CborSerializer::Time)};
	default:
		return {};
	}
}

QList<QCborValue::Type> DateTimeConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	switch (tag) {
	case static_cast<QCborTag>(QCborKnownTags::UnixTime_t):
		return {QCborValue::Integer};
	case static_cast<QCborTag>(QCborKnownTags::DateTimeString):
	case static_cast<QCborTag>(CborSerializer::Date):
	case static_cast<QCborTag>(CborSerializer::Time):
		return {QCborValue::String};
	default:
		if (metaTypeId == QMetaType::QDateTime)
			return {QCborValue::String, QCborValue::Integer};
		else
			return {QCborValue::String};
	}
}

int DateTimeConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	switch (tag) {
	case static_cast<QCborTag>(QCborKnownTags::DateTimeString):
		if (dataType == QCborValue::String)
			return QMetaType::QDateTime;
		else
			break;
	case static_cast<QCborTag>(QCborKnownTags::UnixTime_t):
		if (dataType == QCborValue::Integer)
			return QMetaType::QDateTime;
		else
			break;
	case static_cast<QCborTag>(CborSerializer::Date):
		if (dataType == QCborValue::String)
			return QMetaType::QDate;
		else
			break;
	case static_cast<QCborTag>(CborSerializer::Time):
		if (dataType == QCborValue::String)
			return QMetaType::QTime;
		else
			break;
	default:
		break;
	}

	return QMetaType::UnknownType;
}

QCborValue DateTimeConverter::serialize(int propertyType, const QVariant &value) const
{
	switch (propertyType) {
	case QMetaType::QDateTime:
		if (helper()->getProperty("dateAsTimeStamp").toBool())
			return {QCborKnownTags::UnixTime_t, value.toDateTime().toUTC().toSecsSinceEpoch()};
		else
			return QCborValue{value.toDateTime()};
	case QMetaType::QDate:
		return {static_cast<QCborTag>(CborSerializer::Date), value.toDate().toString(Qt::ISODate)};
	case QMetaType::QTime:
		return {static_cast<QCborTag>(CborSerializer::Time), value.toTime().toString(Qt::ISODateWithMs)};
	default:
		throw SerializationException{"Invalid property type"};
	}
}

QVariant DateTimeConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(parent)
	const auto cValue = (value.isTag() ? value.taggedValue() : value);
	switch (propertyType) {
	case QMetaType::QDateTime:
		return value.toDateTime();
	case QMetaType::QDate:
		if (value.tag() == QCborKnownTags::DateTimeString)
			return value.toDateTime().date();
		else
			return QDate::fromString(cValue.toString(), Qt::ISODate);
	case QMetaType::QTime:
		if (value.tag() == QCborKnownTags::DateTimeString)
			return value.toDateTime().time();
		else
			return QTime::fromString(cValue.toString(), Qt::ISODateWithMs);
	default:
		throw SerializationException{"Invalid property type"};
	}
}

QVariant DateTimeConverter::deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const
{
	if (propertyType == QMetaType::QDateTime &&
		value.type() == QCborValue::String)
		return deserializeCbor(propertyType, {QCborKnownTags::DateTimeString, value}, parent);
	else
		return deserializeCbor(propertyType, value, parent);
}
