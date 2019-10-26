#include "qjsonbytearrayconverter_p.h"
#include "qjsonserializerexception.h"
#include "qjsonserializer.h"

#include <QtCore/QByteArray>
#include <QtCore/QRegularExpression>

bool QJsonBytearrayConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QByteArray;
}

QList<QCborTag> QJsonBytearrayConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {
		NoTag,
		static_cast<QCborTag>(QCborKnownTags::ExpectedBase64),
		static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url),
		static_cast<QCborTag>(QCborKnownTags::ExpectedBase16),
	};
}

QList<QCborValue::Type> QJsonBytearrayConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::ByteArray};
}

int QJsonBytearrayConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	switch (tag) {
	case static_cast<QCborTag>(QCborKnownTags::ExpectedBase64):
	case static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url):
	case static_cast<QCborTag>(QCborKnownTags::ExpectedBase16):
		return QMetaType::QByteArray;
	default:
		return QMetaType::UnknownType;
	}
}

QCborValue QJsonBytearrayConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	return value.toByteArray();
}

QVariant QJsonBytearrayConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	return (value.isTag() ? value.taggedValue() : value).toByteArray();
}

QVariant QJsonBytearrayConverter::deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(parent)

	const auto mode = helper()->getProperty("byteArrayFormat").value<QJsonSerializer::ByteArrayFormat>();
	const auto strValue = value.toString();
	if (helper()->getProperty("validateBase64").toBool()) {
		switch (mode) {
		case QJsonSerializer::ByteArrayFormat::Base64: {
			if ((strValue.size() % 4) != 0)
				throw QJsonDeserializationException("String has invalid length for base64 encoding");
			static const QRegularExpression regex(QStringLiteral(R"__(^[a-zA-Z0-9+\/]*(={0,2})$)__"));
			if (!regex.match(strValue).hasMatch())
				throw QJsonDeserializationException("String contains unallowed symbols for base64 encoding");
			break;
		}
		case QJsonSerializer::ByteArrayFormat::Base64url: {
			static const QRegularExpression regex(QStringLiteral(R"__(^[a-zA-Z0-9\-_]*$)__"));
			if (!regex.match(strValue).hasMatch())
				throw QJsonDeserializationException("String contains unallowed symbols for base64url encoding");
			break;
		}
		case QJsonSerializer::ByteArrayFormat::Base16: {
			if ((strValue.size() % 2) != 0)
				throw QJsonDeserializationException("String has invalid length for base16 encoding");
			static const QRegularExpression regex(QStringLiteral(R"__(^[a-fA-F0-9]*$)__"));
			if (!regex.match(strValue).hasMatch())
				throw QJsonDeserializationException("String contains unallowed symbols for base16 encoding");
			break;
		}
		default:
			Q_UNREACHABLE();
		}
	}

	switch (mode) {
	case QJsonSerializer::ByteArrayFormat::Base64:
		return QByteArray::fromBase64(strValue.toUtf8(), QByteArray::Base64Encoding);
	case QJsonSerializer::ByteArrayFormat::Base64url:
		return QByteArray::fromBase64(strValue.toUtf8(), QByteArray::Base64UrlEncoding);
	case QJsonSerializer::ByteArrayFormat::Base16:
		return QByteArray::fromHex(strValue.toUtf8());
	default:
		Q_UNREACHABLE();
	}
}
