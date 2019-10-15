#include "qjsonchronodurationconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

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

QList<QCborTag> QJsonChronoDurationConverter::allowedCborTags(int metaTypeId) const
{
	QList<QCborTag> tags {
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

QCborValue QJsonChronoDurationConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper)
	if (propertyType == qMetaTypeId<nanoseconds>())
		return {static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds),
				static_cast<qint64>(value.value<nanoseconds>().count())};
	else if (propertyType == qMetaTypeId<microseconds>())
		return {static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds),
				static_cast<qint64>(value.value<microseconds>().count())};
	else if (propertyType == qMetaTypeId<milliseconds>())
		return {static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds),
				static_cast<qint64>(value.value<milliseconds>().count())};
	else if (propertyType == qMetaTypeId<seconds>())
		return {static_cast<QCborTag>(QCborSerializer::ChronoSeconds),
				static_cast<qint64>(value.value<seconds>().count())};
	else if (propertyType == qMetaTypeId<minutes>())
		return {static_cast<QCborTag>(QCborSerializer::ChronoMinutes),
				static_cast<qint64>(value.value<minutes>().count())};
	else if (propertyType == qMetaTypeId<hours>())
		return {static_cast<QCborTag>(QCborSerializer::ChronoHours),
				static_cast<qint64>(value.value<hours>().count())};
	else
		throw QJsonSerializationException{"Invalid type id"};
}

QVariant QJsonChronoDurationConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	Q_UNUSED(helper)

	std::variant<nanoseconds, microseconds, milliseconds, seconds, minutes, hours> metaDuration;
	if (value.isTag()) {
		switch (static_cast<quint64>(value.tag())) {
		case QCborSerializer::ChronoNanoSeconds:
			metaDuration = create<nanoseconds>(value);
			break;
		case QCborSerializer::ChronoMicroSeconds:
			metaDuration = create<microseconds>(value);
			break;
		case QCborSerializer::ChronoMilliSeconds:
			metaDuration = create<milliseconds>(value);
			break;
		case QCborSerializer::ChronoSeconds:
			metaDuration = create<seconds>(value);
			break;
		case QCborSerializer::ChronoMinutes:
			metaDuration = create<minutes>(value);
			break;
		case QCborSerializer::ChronoHours:
			metaDuration = create<hours>(value);
			break;
		default:
			throw QJsonDeserializationException{"Invalid CBOR tag " + QByteArray::number(static_cast<quint64>(value.tag()))};
		}

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
	} else {
		if (propertyType == qMetaTypeId<nanoseconds>())
			return QVariant::fromValue(create<nanoseconds>(value));
		else if (propertyType == qMetaTypeId<microseconds>())
			return QVariant::fromValue(create<microseconds>(value));
		else if (propertyType == qMetaTypeId<milliseconds>())
			return QVariant::fromValue(create<milliseconds>(value));
		else if (propertyType == qMetaTypeId<seconds>())
			return QVariant::fromValue(create<seconds>(value));
		else if (propertyType == qMetaTypeId<minutes>())
			return QVariant::fromValue(create<minutes>(value));
		else if (propertyType == qMetaTypeId<hours>())
			return QVariant::fromValue(create<hours>(value));
		else
			throw QJsonSerializationException{"Invalid type id"};
	}
}
