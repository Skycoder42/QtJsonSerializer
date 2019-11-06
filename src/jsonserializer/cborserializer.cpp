#include "cborserializer.h"
#include "cborserializer_p.h"

#include <QtCore/QCborStreamReader>
#include <QtCore/QCborStreamWriter>
using namespace QtJsonSerializer;

Q_LOGGING_CATEGORY(QtJsonSerializer::logCbor, "qt.jsonserializer.cborserializer")

CborSerializer::CborSerializer(QObject *parent) :
	SerializerBase{*new CborSerializerPrivate{}, parent}
{
	Q_D(CborSerializer);
	d->typeTags = {
		{QMetaType::QColor, static_cast<QCborTag>(CborSerializer::Color)},
		{QMetaType::QFont, static_cast<QCborTag>(CborSerializer::Font)}
	};
}

void CborSerializer::setTypeTag(int metaTypeId, QCborTag tag)
{
	Q_D(CborSerializer);
	Q_ASSERT_X(metaTypeId != QMetaType::UnknownType, Q_FUNC_INFO, "You cannot assign a tag to QMetaType::UnknownType");
	QWriteLocker lock{&d->typeTagsLock};
	if (tag == TypeConverter::NoTag) {
		d->typeTags.remove(metaTypeId);
		qCDebug(logCbor) << "Added Type-Tag for metaTypeId" << metaTypeId
						 << "as" << tag;
	} else {
		d->typeTags.insert(metaTypeId, tag);
		qCDebug(logCbor) << "Removed Type-Tag for metaTypeId" << metaTypeId;
	}
}

QCborTag CborSerializer::typeTag(int metaTypeId) const
{
	Q_D(const CborSerializer);
	QReadLocker lock{&d->typeTagsLock};
	const auto tag = d->typeTags.value(metaTypeId, TypeConverter::NoTag);
	if (tag != TypeConverter::NoTag) {
		qCDebug(logCbor) << "Found Type-Tag for metaTypeId" << metaTypeId
						 << "as" << tag;
	} else
		qCDebug(logCbor) << "No Type-Tag found for metaTypeId" << metaTypeId;
	return tag;
}

QCborValue CborSerializer::serialize(const QVariant &data) const
{
	return serializeVariant(data.userType(), data);
}

void CborSerializer::serializeTo(QIODevice *device, const QVariant &data, QCborValue::EncodingOptions options) const
{
	if (!device->isOpen() || !device->isWritable())
		throw SerializationException{"QIODevice must be open and writable!"};
	QCborStreamWriter writer{device};
	serializeVariant(data.userType(), data).toCbor(writer, options);
}

QByteArray CborSerializer::serializeTo(const QVariant &data, QCborValue::EncodingOptions options) const
{
	return serializeVariant(data.userType(), data).toCbor(options);
}

QVariant CborSerializer::deserialize(const QCborValue &json, int metaTypeId, QObject *parent) const
{
	return deserializeVariant(metaTypeId, json, parent);
}

QVariant CborSerializer::deserializeFrom(QIODevice *device, int metaTypeId, QObject *parent) const
{
	if (!device->isOpen() || !device->isReadable())
		throw DeserializationException{"QIODevice must be open and readable!"};
	QCborStreamReader reader{device};
	const auto cbor = QCborValue::fromCbor(reader);
	if (const auto error = reader.lastError(); error.c != QCborError::NoError)
		throw DeserializationException("Failed to read file as CBOR with error: " + error.toString().toUtf8());
	return deserializeVariant(metaTypeId, cbor, parent);
}

QVariant CborSerializer::deserializeFrom(const QByteArray &data, int metaTypeId, QObject *parent) const
{
	QCborParserError error;
	const auto cbor = QCborValue::fromCbor(data, &error);
	if (error.error.c != QCborError::NoError)
		throw DeserializationException("Failed to read file as CBOR with error: " + error.error.toString().toUtf8());
	return deserializeVariant(metaTypeId, cbor, parent);
}

std::variant<QCborValue, QJsonValue> CborSerializer::serializeGeneric(const QVariant &value) const
{
	return serialize(value);
}

QVariant CborSerializer::deserializeGeneric(const std::variant<QCborValue, QJsonValue> &value, int metaTypeId, QObject *parent) const
{
	return deserialize(std::get<QCborValue>(value), metaTypeId, parent);
}

bool CborSerializer::jsonMode() const
{
	return false;
}

QList<int> CborSerializer::typesForTag(QCborTag tag) const
{
	Q_D(const CborSerializer);
	QReadLocker lock{&d->typeTagsLock};
	const auto keys = d->typeTags.keys(tag);
	qCDebug(logCbor) << "Found metaTypeIds for tag" << tag
					 << "as" << keys;
	return keys;
}
