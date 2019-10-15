#include "qjsonchronodurationconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <chrono>
using namespace std::chrono;

Q_DECLARE_METATYPE(std::chrono::nanoseconds)
Q_DECLARE_METATYPE(std::chrono::microseconds)
Q_DECLARE_METATYPE(std::chrono::milliseconds)
Q_DECLARE_METATYPE(std::chrono::seconds)
Q_DECLARE_METATYPE(std::chrono::minutes)
Q_DECLARE_METATYPE(std::chrono::hours)

bool QJsonChronoDurationConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == qMetaTypeId<nanoseconds>() ||
			metaTypeId == qMetaTypeId<microseconds>() ||
			metaTypeId == qMetaTypeId<milliseconds>() ||
			metaTypeId == qMetaTypeId<seconds>() ||
			metaTypeId == qMetaTypeId<minutes>() ||
			metaTypeId == qMetaTypeId<hours>();
}

QList<QCborTag> QJsonChronoDurationConverter::cborTags() const
{
	return {
		static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds),
		static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds),
		static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds),
		static_cast<QCborTag>(QCborSerializer::ChronoSeconds),
		static_cast<QCborTag>(QCborSerializer::ChronoMinutes),
		static_cast<QCborTag>(QCborSerializer::ChronoHours)
	};
}

QList<QCborValue::Type> QJsonChronoDurationConverter::cborTypes() const
{
	return {QCborValue::Integer};
}

QCborValue QJsonChronoDurationConverter::serializeCbor(int propertyType, const QVariant &value, const QCborTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper)
	if (propertyType == qMetaTypeId<nanoseconds>())
		return QCborValue{tagForType(propertyType), static_cast<qint64>(value.value<nanoseconds>().count())};
	else if (propertyType == qMetaTypeId<microseconds>())
		return QCborValue{tagForType(propertyType), static_cast<qint64>(value.value<microseconds>().count())};
	else if (propertyType == qMetaTypeId<milliseconds>())
		return QCborValue{tagForType(propertyType), static_cast<qint64>(value.value<milliseconds>().count())};
	else if (propertyType == qMetaTypeId<seconds>())
		return QCborValue{tagForType(propertyType), static_cast<qint64>(value.value<seconds>().count())};
	else if (propertyType == qMetaTypeId<minutes>())
		return QCborValue{tagForType(propertyType), static_cast<qint64>(value.value<minutes>().count())};
	else if (propertyType == qMetaTypeId<hours>())
		return QCborValue{tagForType(propertyType), static_cast<qint64>(value.value<hours>().count())};
	else
		throw QJsonSerializationException{"Invalid type id"};
}

QVariant QJsonChronoDurationConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const QCborTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	Q_UNUSED(helper)
	switch (static_cast<quint64>(value.tag())) {
	case QCborSerializer::ChronoNanoSeconds:
		return QVariant::fromValue(nanoseconds{static_cast<nanoseconds::rep>(value.taggedValue().toInteger())});
	case QCborSerializer::ChronoMicroSeconds:
		return QVariant::fromValue(microseconds{static_cast<microseconds::rep>(value.taggedValue().toInteger())});
	case QCborSerializer::ChronoMilliSeconds:
		return QVariant::fromValue(milliseconds{static_cast<milliseconds::rep>(value.taggedValue().toInteger())});
	case QCborSerializer::ChronoSeconds:
		return QVariant::fromValue(seconds{static_cast<seconds::rep>(value.taggedValue().toInteger())});
	case QCborSerializer::ChronoMinutes:
		return QVariant::fromValue(minutes{static_cast<minutes::rep>(value.taggedValue().toInteger())});
	case QCborSerializer::ChronoHours:
		return QVariant::fromValue(hours{static_cast<hours::rep>(value.taggedValue().toInteger())});
	default:
		throw QJsonDeserializationException{"Invalid CBOR tag " + QByteArray::number(static_cast<quint64>(value.tag()))};
	}
}

QVariant QJsonChronoDurationConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper)
	return deserializeCbor(propertyType,
						   QCborValue{tagForType(propertyType), QCborValue::fromJsonValue(value)},
						   parent,
						   nullptr);
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
		return NoTag;
}
