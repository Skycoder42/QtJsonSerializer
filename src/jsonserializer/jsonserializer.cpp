#include "jsonserializer.h"
#include "jsonserializer_p.h"

#include <QtCore/QBuffer>
using namespace QtJsonSerializer;

JsonSerializer::JsonSerializer(QObject *parent) :
	  SerializerBase{*new JsonSerializerPrivate{}, parent}
{}

QJsonValue JsonSerializer::serialize(const QVariant &data) const
{
	return serializeVariant(data.userType(), data).toJsonValue();
}

void JsonSerializer::serializeTo(QIODevice *device, const QVariant &data, QJsonDocument::JsonFormat format) const
{
	if (!device->isOpen() || !device->isWritable())
		throw SerializationException{"QIODevice must be open and writable!"};
	QJsonDocument doc;
	const auto jData = serialize(data);
	if (jData.isArray())
		doc = QJsonDocument{jData.toArray()};
	else if (jData.isObject())
		doc = QJsonDocument{jData.toObject()};
	else
		throw SerializationException{"Only objects or arrays can be written to a device!"};
	device->write(doc.toJson(format));
}

QByteArray JsonSerializer::serializeTo(const QVariant &data, QJsonDocument::JsonFormat format) const
{
	QBuffer buffer;
	if (!buffer.open(QIODevice::WriteOnly))
		throw SerializationException{"Failed to write to bytearray buffer with error: " + buffer.errorString().toUtf8()};
	serializeTo(&buffer, data, format);
	buffer.close();
	return buffer.data();
}

QVariant JsonSerializer::deserialize(const QJsonValue &json, int metaTypeId, QObject *parent) const
{
	return deserializeVariant(metaTypeId, QCborValue::fromJsonValue(json), parent);
}

QVariant JsonSerializer::deserializeFrom(QIODevice *device, int metaTypeId, QObject *parent) const
{
	if (!device->isOpen() || !device->isReadable())
		throw DeserializationException{"QIODevice must be open and readable!"};
	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(device->readAll(), &error);
	if (error.error != QJsonParseError::NoError)
		throw DeserializationException{"Failed to read file as JSON with error: " + error.errorString().toUtf8()};
	if (doc.isArray())
		return deserialize(doc.array(), metaTypeId, parent);
	else if (doc.isObject())
		return deserialize(doc.object(), metaTypeId, parent);
	else if (doc.isNull())
		return deserialize(QJsonValue::Null, metaTypeId, parent);
	else
		return QVariant{};
}

QVariant JsonSerializer::deserializeFrom(const QByteArray &data, int metaTypeId, QObject *parent) const
{
	QBuffer buffer(const_cast<QByteArray*>(&data));
	if (!buffer.open(QIODevice::ReadOnly))
		throw DeserializationException{"Failed to read from bytearray buffer with error: " + buffer.errorString().toUtf8()};
	auto res = deserializeFrom(&buffer, metaTypeId, parent);
	buffer.close();
	return res;
}

JsonSerializer::ByteArrayFormat JsonSerializer::byteArrayFormat() const
{
	Q_D(const JsonSerializer);
	return d->byteArrayFormat;
}

bool JsonSerializer::validateBase64() const
{
	Q_D(const JsonSerializer);
	return d->validateBase64;
}

std::variant<QCborValue, QJsonValue> JsonSerializer::serializeGeneric(const QVariant &value) const
{
	return serialize(value);
}

QVariant JsonSerializer::deserializeGeneric(const std::variant<QCborValue, QJsonValue> &value, int metaTypeId, QObject *parent) const
{
	return deserialize(std::get<QJsonValue>(value), metaTypeId, parent);
}

void JsonSerializer::setByteArrayFormat(JsonSerializer::ByteArrayFormat byteArrayFormat)
{
	Q_D(JsonSerializer);
	if(d->byteArrayFormat == byteArrayFormat)
		return;

	d->byteArrayFormat = byteArrayFormat;
	emit byteArrayFormatChanged(d->byteArrayFormat, {});
}

void JsonSerializer::setValidateBase64(bool validateBase64)
{
	Q_D(JsonSerializer);
	if(d->validateBase64 == validateBase64)
		return;

	d->validateBase64 = validateBase64;
	emit validateBase64Changed(d->validateBase64, {});
}

bool JsonSerializer::jsonMode() const
{
	return true;
}

QCborTag JsonSerializer::typeTag(int metaTypeId) const
{
	Q_D(const JsonSerializer);
	if (metaTypeId == QMetaType::QByteArray) {
		switch (d->byteArrayFormat) {
		case ByteArrayFormat::Base64:
			return static_cast<QCborTag>(QCborKnownTags::ExpectedBase64);
		case ByteArrayFormat::Base64url:
			return static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url);
		case ByteArrayFormat::Base16:
			return static_cast<QCborTag>(QCborKnownTags::ExpectedBase16);
		default:
			Q_UNREACHABLE();
		}
	} else
		return TypeConverter::NoTag;
}

QList<int> JsonSerializer::typesForTag(QCborTag tag) const
{
	Q_UNUSED(tag)
	return {};
}
