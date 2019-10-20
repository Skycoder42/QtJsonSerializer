#include "qjsonchronodurationconverter_p.h"
#include "qcborserializer.h"

#include <QtCore/QSet>

using namespace std::chrono;

Q_DECLARE_METATYPE(std::chrono::nanoseconds)
Q_DECLARE_METATYPE(std::chrono::microseconds)
Q_DECLARE_METATYPE(std::chrono::milliseconds)
Q_DECLARE_METATYPE(std::chrono::seconds)
Q_DECLARE_METATYPE(std::chrono::minutes)
Q_DECLARE_METATYPE(std::chrono::hours)

bool QJsonChronoDurationConverter::canConvert(int metaTypeId) const
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

QList<QCborTag> QJsonChronoDurationConverter::allowedCborTags(int metaTypeId) const
{
	static const QList<QCborTag> tags {
		static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds),
		static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds),
		static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds),
		static_cast<QCborTag>(QCborSerializer::ChronoSeconds),
		static_cast<QCborTag>(QCborSerializer::ChronoMinutes),
		static_cast<QCborTag>(QCborSerializer::ChronoHours)
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

QList<QCborValue::Type> QJsonChronoDurationConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Integer};
}

int QJsonChronoDurationConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	if (dataType != QCborValue::Integer)
		return QMetaType::UnknownType;

	switch (static_cast<quint64>(tag)) {
	case QCborSerializer::ChronoNanoSeconds:
		return qMetaTypeId<nanoseconds>();
	case QCborSerializer::ChronoMicroSeconds:
		return qMetaTypeId<microseconds>();
	case QCborSerializer::ChronoMilliSeconds:
		return qMetaTypeId<milliseconds>();
	case QCborSerializer::ChronoSeconds:
		return qMetaTypeId<seconds>();
	case QCborSerializer::ChronoMinutes:
		return qMetaTypeId<minutes>();
	case QCborSerializer::ChronoHours:
		return qMetaTypeId<hours>();
	default:
		return QMetaType::UnknownType;
	}
}

QCborValue QJsonChronoDurationConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto tag = tagForType(propertyType);
	switch (static_cast<quint64>(tag)) {
	case QCborSerializer::ChronoNanoSeconds:
		return {tag, static_cast<qint64>(value.value<nanoseconds>().count())};
	case QCborSerializer::ChronoMicroSeconds:
		return {tag, static_cast<qint64>(value.value<microseconds>().count())};
	case QCborSerializer::ChronoMilliSeconds:
		return {tag, static_cast<qint64>(value.value<milliseconds>().count())};
	case QCborSerializer::ChronoSeconds:
		return {tag, static_cast<qint64>(value.value<seconds>().count())};
	case QCborSerializer::ChronoMinutes:
		return {tag, static_cast<qint64>(value.value<minutes>().count())};
	case QCborSerializer::ChronoHours:
		return {tag, static_cast<qint64>(value.value<hours>().count())};
	default:
		Q_UNREACHABLE();
	}
}

QVariant QJsonChronoDurationConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
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
		throw QJsonSerializationException{"Invalid type id"};
}

QCborTag QJsonChronoDurationConverter::tagForType(int metaTypeId) const
{
	if (metaTypeId == qMetaTypeId<nanoseconds>())
		return static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds);
	else if (metaTypeId == qMetaTypeId<microseconds>())
		return static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds);
	else if (metaTypeId == qMetaTypeId<milliseconds>())
		return static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds);
	else if (metaTypeId == qMetaTypeId<seconds>())
		return static_cast<QCborTag>(QCborSerializer::ChronoSeconds);
	else if (metaTypeId == qMetaTypeId<minutes>())
		return static_cast<QCborTag>(QCborSerializer::ChronoMinutes);
	else if (metaTypeId == qMetaTypeId<hours>())
		return static_cast<QCborTag>(QCborSerializer::ChronoHours);
	else
		throw QJsonSerializationException{"Invalid type id"};
}

QJsonChronoDurationConverter::MetaDuration QJsonChronoDurationConverter::parseValue(int propertyType, const QCborValue &value, const QJsonTypeConverter::SerializationHelper *localHelper) const
{
	const auto rTag = value.isTag() ? value.tag() : NoTag;
	switch (static_cast<quint64>(rTag)) {
	case QCborSerializer::ChronoNanoSeconds:
		return create<nanoseconds>(value);
	case QCborSerializer::ChronoMicroSeconds:
		return create<microseconds>(value);
	case QCborSerializer::ChronoMilliSeconds:
		return create<milliseconds>(value);
	case QCborSerializer::ChronoSeconds:
		return create<seconds>(value);
	case QCborSerializer::ChronoMinutes:
		return create<minutes>(value);
	case QCborSerializer::ChronoHours:
		return create<hours>(value);
	default:
		if (!localHelper || rTag != localHelper->typeTag(propertyType))
			throw QJsonDeserializationException{"Invalid CBOR tag " + QByteArray::number(static_cast<quint64>(value.tag()))};
		Q_FALLTHROUGH();
	case static_cast<quint64>(NoTag):
		if (auto vTag = tagForType(propertyType); vTag != NoTag)
			return parseValue(QMetaType::UnknownType, {vTag, value.isTag() ? value.taggedValue() : value}, nullptr);
		else
			throw QJsonSerializationException{"Invalid state reached - no CBOR tag was given, but the property type is not a chrono type"};
	}
}
