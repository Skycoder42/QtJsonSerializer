#include "qcborserializer.h"
#include "qcborserializer_p.h"

#include <QtCore/QCborStreamReader>
#include <QtCore/QCborStreamWriter>

#if !defined(__MINGW64__) && !defined(__MINGW32__)
static_assert(QJsonTypeConverter::NoTag == static_cast<QCborTag>(QCborSerializer::NoTag));
#endif

QCborSerializer::QCborSerializer(QObject *parent) :
	QJsonSerializerBase{*new QCborSerializerPrivate{}, parent}
{
	Q_D(QCborSerializer);
	d->typeTags = {
		{QMetaType::QColor, static_cast<QCborTag>(QCborSerializer::Color)},
		{QMetaType::QFont, static_cast<QCborTag>(QCborSerializer::Font)}
	};
}

void QCborSerializer::setTypeTag(int metaTypeId, QCborTag tag)
{
	Q_D(QCborSerializer);
	Q_ASSERT_X(metaTypeId != QMetaType::UnknownType, Q_FUNC_INFO, "You cannot assign a tag to QMetaType::UnknownType");
	QWriteLocker lock{&d->typeTagsLock};
	if (tag == QJsonTypeConverter::NoTag)
		d->typeTags.remove(metaTypeId);
	else
		d->typeTags.insert(metaTypeId, tag);
}

QCborTag QCborSerializer::typeTag(int metaTypeId) const
{
	Q_D(const QCborSerializer);
	QReadLocker lock{&d->typeTagsLock};
	return d->typeTags.value(metaTypeId, QJsonTypeConverter::NoTag);
}

QCborValue QCborSerializer::serialize(const QVariant &data) const
{
	return serializeVariant(data.userType(), data);
}

void QCborSerializer::serializeTo(QIODevice *device, const QVariant &data, QCborValue::EncodingOptions options) const
{
	QCborStreamWriter writer{device};
	serializeVariant(data.userType(), data).toCbor(writer, options);
}

QByteArray QCborSerializer::serializeTo(const QVariant &data, QCborValue::EncodingOptions options) const
{
	return serializeVariant(data.userType(), data).toCbor(options);
}

QVariant QCborSerializer::deserialize(const QCborValue &json, int metaTypeId, QObject *parent) const
{
	return deserializeVariant(metaTypeId, json, parent);
}

QVariant QCborSerializer::deserializeFrom(QIODevice *device, int metaTypeId, QObject *parent) const
{
	QCborStreamReader reader{device};
	const auto cbor = QCborValue::fromCbor(reader);
	if (const auto error = reader.lastError(); error.c != QCborError::NoError)
		throw QJsonDeserializationException("Failed to read file as CBOR with error: " + error.toString().toUtf8());
	return deserializeVariant(metaTypeId, cbor, parent);
}

QVariant QCborSerializer::deserializeFrom(const QByteArray &data, int metaTypeId, QObject *parent) const
{
	QCborParserError error;
	const auto cbor = QCborValue::fromCbor(data, &error);
	if (error.error.c != QCborError::NoError)
		throw QJsonDeserializationException("Failed to read file as CBOR with error: " + error.error.toString().toUtf8());
	return deserializeVariant(metaTypeId, cbor, parent);
}

std::variant<QCborValue, QJsonValue> QCborSerializer::serializeGeneric(const QVariant &value) const
{
	return serialize(value);
}

QVariant QCborSerializer::deserializeGeneric(const std::variant<QCborValue, QJsonValue> &value, int metaTypeId, QObject *parent) const
{
	return deserialize(std::get<QCborValue>(value), metaTypeId, parent);
}

bool QCborSerializer::jsonMode() const
{
	return false;
}

QList<int> QCborSerializer::typesForTag(QCborTag tag) const
{
	Q_D(const QCborSerializer);
	QReadLocker lock{&d->typeTagsLock};
	return d->typeTags.keys(tag);
}
