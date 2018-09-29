#include "qjsonlocaleconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QLocale>

bool QJsonLocaleConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QLocale;
}

QList<QJsonValue::Type> QJsonLocaleConverter::jsonTypes() const
{
	return {QJsonValue::String};
}

QJsonValue QJsonLocaleConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)

	if(helper->getProperty("useBcp47Locale").toBool())
		return value.toLocale().bcp47Name();
	else
		return value.toLocale().name();
}

QVariant QJsonLocaleConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	Q_UNUSED(helper)

	auto strValue = value.toString();
	QLocale locale(strValue);
	if(locale == QLocale::c()) {
		if(strValue.toUpper() == QStringLiteral("C") ||
		   strValue.isEmpty())
			return QLocale::c();
		else
			throw QJsonDeserializationException("String cannot be interpreted as locale");
	} else
		return locale;
}
