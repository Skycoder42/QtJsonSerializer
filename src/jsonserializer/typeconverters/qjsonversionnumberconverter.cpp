#include "qjsonversionnumberconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QVersionNumber>
#include <QtCore/QDebug>

bool QJsonVersionNumberConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == qMetaTypeId<QVersionNumber>();
}

QList<QJsonValue::Type> QJsonVersionNumberConverter::jsonTypes() const
{
	return {QJsonValue::String};
}

QJsonValue QJsonVersionNumberConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(helper)
	return value.value<QVersionNumber>().toString();
}

QVariant QJsonVersionNumberConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	Q_UNUSED(helper)

	auto strValue = value.toString();
	if(!strValue.isEmpty()) {
		int suffixIndex = -1;
		auto version = QVersionNumber::fromString(value.toString(), &suffixIndex);
		if(version.isNull())
			throw QJsonDeserializationException("Invalid version number, no segments found");
		if(suffixIndex < strValue.size())
			qWarning() << "Parsed QVersionNumber with suffix - suffixes are discarded!";
		return QVariant::fromValue(version);
	} else
		return QVariant::fromValue<QVersionNumber>({});
}
