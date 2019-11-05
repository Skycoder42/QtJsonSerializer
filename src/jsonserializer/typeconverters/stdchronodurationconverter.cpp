#include "stdchronodurationconverter_p.h"
#include "cborserializer.h"

#include <QtCore/QSet>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

using namespace std::chrono;

Q_DECLARE_METATYPE(std::chrono::nanoseconds)
Q_DECLARE_METATYPE(std::chrono::microseconds)
Q_DECLARE_METATYPE(std::chrono::milliseconds)
Q_DECLARE_METATYPE(std::chrono::seconds)
Q_DECLARE_METATYPE(std::chrono::minutes)
Q_DECLARE_METATYPE(std::chrono::hours)

bool StdChronoDurationConverter::canConvert(int metaTypeId) const
{
	static const QSet<int> metaTypeIdList {
		qMetaTypeId<nanoseconds>(),
		qMetaTypeId<microseconds>(),
		qMetaTypeId<milliseconds>(),
		qMetaTypeId<seconds>(),
		qMetaTypeId<minutes>(),
		qMetaTypeId<hours>()
	};
	return metaTypeIdList.contains(metaTypeId);
}

QList<QCborTag> StdChronoDurationConverter::allowedCborTags(int metaTypeId) const
{
	static const QList<QCborTag> tags {
		static_cast<QCborTag>(CborSerializer::ChronoNanoSeconds),
		static_cast<QCborTag>(CborSerializer::ChronoMicroSeconds),
		static_cast<QCborTag>(CborSerializer::ChronoMilliSeconds),
		static_cast<QCborTag>(CborSerializer::ChronoSeconds),
		static_cast<QCborTag>(CborSerializer::ChronoMinutes),
		static_cast<QCborTag>(CborSerializer::ChronoHours)
	};

	if (metaTypeId == qMetaTypeId<nanoseconds>())
		return tags;
	else if (metaTypeId == qMetaTypeId<microseconds>())
		return tags.mid(1);
	else if (metaTypeId == qMetaTypeId<milliseconds>())
		return tags.mid(2);
	else if (metaTypeId == qMetaTypeId<seconds>())
		return tags.mid(3);
	else if (metaTypeId == qMetaTypeId<minutes>())
		return tags.mid(4);
	else if (metaTypeId == qMetaTypeId<hours>())
		return tags.mid(5);
	else
		Q_UNREACHABLE();
}

QList<QCborValue::Type> StdChronoDurationConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Integer};
}

int StdChronoDurationConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	if (dataType != QCborValue::Integer)
		return QMetaType::UnknownType;

	switch (static_cast<quint64>(tag)) {
	case CborSerializer::ChronoNanoSeconds:
		return qMetaTypeId<nanoseconds>();
	case CborSerializer::ChronoMicroSeconds:
		return qMetaTypeId<microseconds>();
	case CborSerializer::ChronoMilliSeconds:
		return qMetaTypeId<milliseconds>();
	case CborSerializer::ChronoSeconds:
		return qMetaTypeId<seconds>();
	case CborSerializer::ChronoMinutes:
		return qMetaTypeId<minutes>();
	case CborSerializer::ChronoHours:
		return qMetaTypeId<hours>();
	default:
		return QMetaType::UnknownType;
	}
}

QCborValue StdChronoDurationConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto tag = tagForType(propertyType);
	switch (static_cast<quint64>(tag)) {
	case CborSerializer::ChronoNanoSeconds:
		return {tag, static_cast<qint64>(value.value<nanoseconds>().count())};
	case CborSerializer::ChronoMicroSeconds:
		return {tag, static_cast<qint64>(value.value<microseconds>().count())};
	case CborSerializer::ChronoMilliSeconds:
		return {tag, static_cast<qint64>(value.value<milliseconds>().count())};
	case CborSerializer::ChronoSeconds:
		return {tag, static_cast<qint64>(value.value<seconds>().count())};
	case CborSerializer::ChronoMinutes:
		return {tag, static_cast<qint64>(value.value<minutes>().count())};
	case CborSerializer::ChronoHours:
		return {tag, static_cast<qint64>(value.value<hours>().count())};
	default:
		Q_UNREACHABLE();
	}
}

QVariant StdChronoDurationConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(parent)
	const auto metaDuration = parseValue(propertyType, value, helper());
	if (propertyType == qMetaTypeId<nanoseconds>())
		return QVariant::fromValue(cast<nanoseconds>(metaDuration));
	else if (propertyType == qMetaTypeId<microseconds>())
		return QVariant::fromValue(cast<microseconds>(metaDuration));
	else if (propertyType == qMetaTypeId<milliseconds>())
		return QVariant::fromValue(cast<milliseconds>(metaDuration));
	else if (propertyType == qMetaTypeId<seconds>())
		return QVariant::fromValue(cast<seconds>(metaDuration));
	else if (propertyType == qMetaTypeId<minutes>())
		return QVariant::fromValue(cast<minutes>(metaDuration));
	else if (propertyType == qMetaTypeId<hours>())
		return QVariant::fromValue(cast<hours>(metaDuration));
	else
		throw SerializationException{"Invalid type id"};
}

QCborTag StdChronoDurationConverter::tagForType(int metaTypeId) const
{
	if (metaTypeId == qMetaTypeId<nanoseconds>())
		return static_cast<QCborTag>(CborSerializer::ChronoNanoSeconds);
	else if (metaTypeId == qMetaTypeId<microseconds>())
		return static_cast<QCborTag>(CborSerializer::ChronoMicroSeconds);
	else if (metaTypeId == qMetaTypeId<milliseconds>())
		return static_cast<QCborTag>(CborSerializer::ChronoMilliSeconds);
	else if (metaTypeId == qMetaTypeId<seconds>())
		return static_cast<QCborTag>(CborSerializer::ChronoSeconds);
	else if (metaTypeId == qMetaTypeId<minutes>())
		return static_cast<QCborTag>(CborSerializer::ChronoMinutes);
	else if (metaTypeId == qMetaTypeId<hours>())
		return static_cast<QCborTag>(CborSerializer::ChronoHours);
	else
		throw SerializationException{"Invalid type id"};
}

StdChronoDurationConverter::MetaDuration StdChronoDurationConverter::parseValue(int propertyType, const QCborValue &value, const TypeConverter::SerializationHelper *localHelper) const
{
	const auto rTag = value.isTag() ? value.tag() : NoTag;
	switch (static_cast<quint64>(rTag)) {
	case CborSerializer::ChronoNanoSeconds:
		return create<nanoseconds>(value);
	case CborSerializer::ChronoMicroSeconds:
		return create<microseconds>(value);
	case CborSerializer::ChronoMilliSeconds:
		return create<milliseconds>(value);
	case CborSerializer::ChronoSeconds:
		return create<seconds>(value);
	case CborSerializer::ChronoMinutes:
		return create<minutes>(value);
	case CborSerializer::ChronoHours:
		return create<hours>(value);
	default:
		if (!localHelper || rTag != localHelper->typeTag(propertyType))
			throw DeserializationException{"Invalid CBOR tag " + QByteArray::number(static_cast<quint64>(value.tag()))};
		Q_FALLTHROUGH();
	case static_cast<quint64>(NoTag):
		if (auto vTag = tagForType(propertyType); vTag != NoTag)
			return parseValue(QMetaType::UnknownType, {vTag, value.isTag() ? value.taggedValue() : value}, nullptr);
		else
			throw SerializationException{"Invalid state reached - no CBOR tag was given, but the property type is not a chrono type"};
	}
}
