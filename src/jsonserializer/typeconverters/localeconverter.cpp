#include "localeconverter_p.h"
#include "exception.h"
#include "cborserializer.h"

#include <QtCore/QLocale>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool LocaleConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QLocale;
}

QList<QCborTag> LocaleConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {
		static_cast<QCborTag>(CborSerializer::LocaleISO),
		static_cast<QCborTag>(CborSerializer::LocaleBCP47),
	};
}

QList<QCborValue::Type> LocaleConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::String};
}

int LocaleConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	return allowedCborTags(QMetaType::UnknownType).contains(tag) &&
		   dataType == QCborValue::String;
}

QCborValue LocaleConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	if (helper()->getProperty("useBcp47Locale").toBool())
		return {static_cast<QCborTag>(CborSerializer::LocaleBCP47), value.toLocale().bcp47Name()};
	else
		return {static_cast<QCborTag>(CborSerializer::LocaleISO), value.toLocale().name()};
}

QVariant LocaleConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
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
			throw DeserializationException("String cannot be interpreted as locale");
	} else
		return locale;
}
