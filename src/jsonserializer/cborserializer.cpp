#include "cborserializer.h"
#include "cborserializer_p.h"

#include <cmath>

#include <QtCore/QCborStreamReader>
#include <QtCore/QCborStreamWriter>
#include <QtCore/QtEndian>
using namespace QtJsonSerializer;

Q_LOGGING_CATEGORY(QtJsonSerializer::logCbor, "qt.jsonserializer.serializer.cbor")

CborSerializer::CborSerializer(QObject *parent) :
	SerializerBase{*new CborSerializerPrivate{}, parent}
{
	Q_D(CborSerializer);
	d->typeTags = {
		{QMetaType::QColor, static_cast<QCborTag>(CborSerializer::Color)},
		{QMetaType::QFont, static_cast<QCborTag>(CborSerializer::Font)}
	};
}

bool CborSerializer::handleSpecialNumbers() const
{
	Q_D(const CborSerializer);
	return d->handleSpecialNumbers;
}

void CborSerializer::setTypeTag(int metaTypeId, QCborTag tag)
{
	Q_D(CborSerializer);
	Q_ASSERT_X(metaTypeId != QMetaType::UnknownType, Q_FUNC_INFO, "You cannot assign a tag to QMetaType::UnknownType");
	QWriteLocker lock{&d->typeTagsLock};
	if (tag == TypeConverter::NoTag) {
		d->typeTags.remove(metaTypeId);
		qCDebug(logCbor) << "Added Type-Tag for" << QMetaType::typeName(metaTypeId)
						 << "as" << tag;
	} else {
		d->typeTags.insert(metaTypeId, tag);
		qCDebug(logCbor) << "Removed Type-Tag for metaTypeId" << QMetaType::typeName(metaTypeId);
	}
}

QCborTag CborSerializer::typeTag(int metaTypeId) const
{
	Q_D(const CborSerializer);
	QReadLocker lock{&d->typeTagsLock};
	const auto tag = d->typeTags.value(metaTypeId, TypeConverter::NoTag);
	if (tag != TypeConverter::NoTag) {
		qCDebug(logCbor) << "Found Type-Tag for metaTypeId" << QMetaType::typeName(metaTypeId)
						 << "as" << tag;
	} else
		qCDebug(logCbor) << "No Type-Tag found for metaTypeId" << QMetaType::typeName(metaTypeId);
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

void CborSerializer::setHandleSpecialNumbers(bool handleSpecialNumbers)
{
	Q_D(CborSerializer);
	if(d->handleSpecialNumbers == handleSpecialNumbers)
		return;

	d->handleSpecialNumbers = handleSpecialNumbers;
	emit handleSpecialNumbersChanged(d->handleSpecialNumbers, {});
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

// ------------- private implementation -------------

namespace {

template <typename TInt>
bool testOverflow(const QByteArray &data) {
	return (data.size() == static_cast<int>(sizeof(TInt))) &&
		   (data[0] & 0x80) != 0;
}

template <typename TInt, bool Invert = false>
TInt extract(QByteArray data) {
	static_assert (std::is_integral_v<TInt>, "TInt must be an integer type");
	if (data.size() < static_cast<int>(sizeof(TInt)))
		data.prepend(QByteArray(static_cast<int>(sizeof(TInt)) - data.size(), 0));
	if constexpr (Invert)
		return ~qFromBigEndian<TInt>(data.data());
	else
		return qFromBigEndian<TInt>(data.data());
}

}

QVariant CborSerializerPrivate::deserializeCborValue(int propertyType, const QCborValue &value) const
{
	if (handleSpecialNumbers) {
		switch (value.tag()) {
		case static_cast<QCborTag>(QCborKnownTags::PositiveBignum):
			return deserializePositiveBignum(value.taggedValue().toByteArray());
		case static_cast<QCborTag>(QCborKnownTags::NegativeBignum):
			return deserializeNegativeBignum(value.taggedValue().toByteArray());
		case static_cast<QCborTag>(QCborKnownTags::Decimal):
			return deserializeDecimal(value.taggedValue().toArray());
		case static_cast<QCborTag>(QCborKnownTags::Bigfloat):
			return deserializeBigfloat(value.taggedValue().toArray());
		case static_cast<QCborTag>(ExtendedTags::RationaleNumber):
			return deserializeRationaleNumber(value.taggedValue().toArray());
		default:
			break;
		}
	}

	return SerializerBasePrivate::deserializeCborValue(propertyType, value);
}

QVariant CborSerializerPrivate::deserializePositiveBignum(const QByteArray &data) const
{
	const auto dSize = static_cast<size_t>(data.size());
	if (dSize > static_cast<int>(sizeof(quint64))) {
		throw DeserializationException{"Unable to handle PositiveBignum tagged integers, bigger then " +
									   QByteArray::number(static_cast<int>(sizeof(quint64))) +
									   " bytes"};
	}
	if (dSize <= sizeof (quint8))
		return extract<quint8>(data);
	else if (dSize <= sizeof (quint16))
		return extract<quint16>(data);
	else if (dSize <= sizeof (quint32))
		return extract<quint32>(data);
	else
		return QVariant::fromValue(extract<quint64>(data));
}

QVariant CborSerializerPrivate::deserializeNegativeBignum(const QByteArray &data) const
{
	const auto dSize = static_cast<size_t>(data.size());
	if (dSize > static_cast<int>(sizeof(qint64)) ||  testOverflow<qint64>(data)) {
		throw DeserializationException{"Unable to handle NegativeBignum tagged integers, bigger then " +
									   QByteArray::number(static_cast<int>(sizeof(qint64))) +
									   " bytes (- the first bit)"};
	}
	if (dSize <= sizeof (qint8) && !testOverflow<qint8>(data))
		return QVariant::fromValue<qint8>(extract<qint8, true>(data));
	else if (dSize <= sizeof (qint16) && !testOverflow<qint16>(data))
		return extract<qint16, true>(data);
	else if (dSize <= sizeof (qint32) && !testOverflow<qint32>(data))
		return extract<qint32, true>(data);
	else
		return QVariant::fromValue(extract<qint64, true>(data));
}

qreal CborSerializerPrivate::deserializeDecimal(const QCborArray &data) const
{
	if (data.size() != 2)
		throw DeserializationException{"Decimal tagged types must be an array with exactly two elements"};
	return std::pow(10, data[0].toInteger()) * data[1].toInteger();
}

qreal CborSerializerPrivate::deserializeBigfloat(const QCborArray &data) const
{
	if (data.size() != 2)
		throw DeserializationException{"Bigfloat tagged types must be an array with exactly two elements"};
	return std::ldexp(data[1].toInteger(), data[0].toInteger());
}

qreal CborSerializerPrivate::deserializeRationaleNumber(const QCborArray &data) const
{
	if (data.size() != 2)
		throw DeserializationException{"RationaleNumber tagged types must be an array with exactly two elements"};
	return static_cast<long double>(data[0].toInteger()) /
		   static_cast<long double>(data[1].toInteger());
}
