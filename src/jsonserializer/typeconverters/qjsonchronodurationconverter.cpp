#include "qjsonchronodurationconverter_p.h"
#include "qjsonserializerexception.h"

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

QList<QJsonValue::Type> QJsonChronoDurationConverter::jsonTypes() const
{
	return {QJsonValue::Double};
}

QJsonValue QJsonChronoDurationConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *) const
{
	if (propertyType == qMetaTypeId<nanoseconds>())
		return static_cast<qint64>(value.value<nanoseconds>().count());
	else if (propertyType == qMetaTypeId<microseconds>())
		return static_cast<qint64>(value.value<microseconds>().count());
	else if (propertyType == qMetaTypeId<milliseconds>())
		return static_cast<qint64>(value.value<milliseconds>().count());
	else if (propertyType == qMetaTypeId<seconds>())
		return static_cast<qint64>(value.value<seconds>().count());
	else if (propertyType == qMetaTypeId<minutes>())
		return static_cast<qint64>(value.value<minutes>().count());
	else if (propertyType == qMetaTypeId<hours>())
		return static_cast<qint64>(value.value<hours>().count());
	else
		throw QJsonSerializationException{"Invalid type id"};
}

QVariant QJsonChronoDurationConverter::deserialize(int propertyType, const QJsonValue &value, QObject *, const QJsonTypeConverter::SerializationHelper *) const
{
	if (propertyType == qMetaTypeId<nanoseconds>())
		return QVariant::fromValue(nanoseconds{static_cast<nanoseconds::rep>(value.toDouble())});
	else if (propertyType == qMetaTypeId<microseconds>())
		return QVariant::fromValue(microseconds{static_cast<microseconds::rep>(value.toDouble())});
	else if (propertyType == qMetaTypeId<milliseconds>())
		return QVariant::fromValue(milliseconds{static_cast<milliseconds::rep>(value.toDouble())});
	else if (propertyType == qMetaTypeId<seconds>())
		return QVariant::fromValue(seconds{static_cast<seconds::rep>(value.toDouble())});
	else if (propertyType == qMetaTypeId<minutes>())
		return QVariant::fromValue(minutes{static_cast<minutes::rep>(value.toDouble())});
	else if (propertyType == qMetaTypeId<hours>())
		return QVariant::fromValue(hours{static_cast<hours::rep>(value.toDouble())});
	else
		throw QJsonDeserializationException{"Invalid type id"};
}
