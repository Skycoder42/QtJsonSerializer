#include "qjsonbytearrayconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QByteArray>
#include <QtCore/QRegularExpression>

bool QJsonBytearrayConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QByteArray;
}

QList<QJsonValue::Type> QJsonBytearrayConverter::jsonTypes() const
{
	return {QJsonValue::String};
}

QJsonValue QJsonBytearrayConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(helper)

	return QString::fromUtf8(value.toByteArray().toBase64());
}

QVariant QJsonBytearrayConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)

	auto validateBase64 = helper->getProperty("validateBase64").toBool();
	auto strValue = value.toString();
	if(validateBase64) {
		if((strValue.size() % 4) != 0)
			throw QJsonDeserializationException("String has invalid length for base64 encoding");
		static const QRegularExpression regex(QStringLiteral(R"__(^[a-zA-Z0-9+\/]*(={0,2})$)__"));
		if(!regex.match(strValue).hasMatch())
			throw QJsonDeserializationException("String contains unallowed symbols for base64 encoding");
	}

	return QByteArray::fromBase64(strValue.toUtf8());
}
