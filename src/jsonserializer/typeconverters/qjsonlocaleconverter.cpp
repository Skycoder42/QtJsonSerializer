#include "qjsonlocaleconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <QtCore/QLocale>

bool QJsonLocaleConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QLocale;
}

QList<QCborTag> QJsonLocaleConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {
		static_cast<QCborTag>(QCborSerializer::LocaleISO),
		static_cast<QCborTag>(QCborSerializer::LocaleBCP47),
	};
}

QList<QCborValue::Type> QJsonLocaleConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::String};
}

int QJsonLocaleConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	return allowedCborTags(QMetaType::UnknownType).contains(tag) &&
		   dataType == QCborValue::String;
}

QCborValue QJsonLocaleConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	if (helper()->getProperty("useBcp47Locale").toBool())
		return {static_cast<QCborTag>(QCborSerializer::LocaleBCP47), value.toLocale().bcp47Name()};
	else
		return {static_cast<QCborTag>(QCborSerializer::LocaleISO), value.toLocale().name()};
}

QVariant QJsonLocaleConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)

	const auto strValue = (value.isTag() ? value.taggedValue() : value).toString();
	QLocale locale{strValue};
	if (locale == QLocale::c()) {
		if (strValue.toUpper() == QLatin1Char('C') ||
			strValue.isEmpty())
			return QLocale::c();
		else
			throw QJsonDeserializationException("String cannot be interpreted as locale");
	} else
		return locale;
}
