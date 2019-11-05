#include "bytearrayconverter_p.h"
#include "exception.h"
#include "jsonserializer.h"

#include <QtCore/QByteArray>
#include <QtCore/QRegularExpression>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool BytearrayConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QByteArray;
}

QList<QCborTag> BytearrayConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {
		NoTag,
		static_cast<QCborTag>(QCborKnownTags::ExpectedBase64),
		static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url),
		static_cast<QCborTag>(QCborKnownTags::ExpectedBase16),
	};
}

QList<QCborValue::Type> BytearrayConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::ByteArray};
}

int BytearrayConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	Q_UNUSED(dataType)
	switch (tag) {
	case static_cast<QCborTag>(QCborKnownTags::ExpectedBase64):
	case static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url):
	case static_cast<QCborTag>(QCborKnownTags::ExpectedBase16):
		return QMetaType::QByteArray;
	default:
		return QMetaType::UnknownType;
	}
}

QCborValue BytearrayConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	return value.toByteArray();
}

QVariant BytearrayConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	return (value.isTag() ? value.taggedValue() : value).toByteArray();
}

QVariant BytearrayConverter::deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)

	const auto mode = helper()->getProperty("byteArrayFormat").value<JsonSerializer::ByteArrayFormat>();
	const auto strValue = value.toString();
	if (helper()->getProperty("validateBase64").toBool()) {
		switch (mode) {
		case JsonSerializer::ByteArrayFormat::Base64: {
			if ((strValue.size() % 4) != 0)
				throw DeserializationException("String has invalid length for base64 encoding");
			static const QRegularExpression regex(QStringLiteral(R"__(^[a-zA-Z0-9+\/]*(={0,2})$)__"));
			if (!regex.match(strValue).hasMatch())
				throw DeserializationException("String contains unallowed symbols for base64 encoding");
			break;
		}
		case JsonSerializer::ByteArrayFormat::Base64url: {
			static const QRegularExpression regex(QStringLiteral(R"__(^[a-zA-Z0-9\-_]*$)__"));
			if (!regex.match(strValue).hasMatch())
				throw DeserializationException("String contains unallowed symbols for base64url encoding");
			break;
		}
		case JsonSerializer::ByteArrayFormat::Base16: {
			if ((strValue.size() % 2) != 0)
				throw DeserializationException("String has invalid length for base16 encoding");
			static const QRegularExpression regex(QStringLiteral(R"__(^[a-fA-F0-9]*$)__"));
			if (!regex.match(strValue).hasMatch())
				throw DeserializationException("String contains unallowed symbols for base16 encoding");
			break;
		}
		default:
			Q_UNREACHABLE();
		}
	}

	switch (mode) {
	case JsonSerializer::ByteArrayFormat::Base64:
		return QByteArray::fromBase64(strValue.toUtf8(), QByteArray::Base64Encoding);
	case JsonSerializer::ByteArrayFormat::Base64url:
		return QByteArray::fromBase64(strValue.toUtf8(), QByteArray::Base64UrlEncoding);
	case JsonSerializer::ByteArrayFormat::Base16:
		return QByteArray::fromHex(strValue.toUtf8());
	default:
		Q_UNREACHABLE();
	}
}
