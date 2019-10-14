#include "qcborserializer.h"
#include "qcborserializer_p.h"
#include "qjsonexceptioncontext_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QCborStreamReader>
#include <QtCore/QCborStreamWriter>

#include "typeconverters/qjsonenumconverter_p.h"

#define D (static_cast<QCborSerializerPrivate*>(d.data()))

QCborSerializer::QCborSerializer(QObject *parent) :
	QJsonSerializer{new QCborSerializerPrivate{}, parent}
{
	D->typeTags = {
		{QMetaType::QColor, static_cast<QCborTag>(QCborSerializer::Color)},
		{QMetaType::QFont, static_cast<QCborTag>(QCborSerializer::Font)}
	};
}

void QCborSerializer::setTypeTag(int metaTypeId, QCborTag tag)
{
	QWriteLocker lock{&D->typeTagsLock};
	if (tag == QCborTypeConverter::NoTag)
		D->typeTags.remove(metaTypeId);
	else
		D->typeTags.insert(metaTypeId, tag);
}

QCborTag QCborSerializer::typeTag(int metaTypeId) const
{
	QReadLocker lock{&D->typeTagsLock};
	return D->typeTags.value(metaTypeId, QCborTypeConverter::NoTag);
}

QCborSerializer::~QCborSerializer() = default;

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

QCborSerializer::ByteArrayTag QCborSerializer::byteArrayTag() const
{
	return D->byteArrayTag;
}

void QCborSerializer::setByteArrayTag(QCborSerializer::ByteArrayTag byteArrayTag)
{
	if (D->byteArrayTag == byteArrayTag)
		return;

	D->byteArrayTag = byteArrayTag;
	emit byteArrayTagChanged(D->byteArrayTag, {});
}

QCborValue QCborSerializer::serializeCborSubtype(const QMetaProperty &property, const QVariant &value) const
{
	QJsonExceptionContext ctx{property};
	if (property.isEnumType())  // TODO implement properly
		return QCborValue::fromJsonValue(QJsonEnumConverter::serializeEnum(property.enumerator(), value, d->enumAsString));
	else
		return serializeVariant(property.userType(), value);
}

QCborValue QCborSerializer::serializeCborSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx{propertyType, traceHint};
	return serializeVariant(propertyType, value);
}

QVariant QCborSerializer::deserializeCborSubtype(const QMetaProperty &property, const QCborValue &value, QObject *parent) const
{
	QJsonExceptionContext ctx{property};
	if(property.isEnumType())  // TODO implement properly
		return QJsonEnumConverter::deserializeEnum(property.enumerator(), value.toJsonValue());
	else
		return deserializeVariant(property.userType(), value, parent);
}

QVariant QCborSerializer::deserializeCborSubtype(int propertyType, const QCborValue &value, QObject *parent, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx{propertyType, traceHint};
	return deserializeVariant(propertyType, value, parent);
}

QCborValue QCborSerializer::serializeVariant(int propertyType, const QVariant &value) const
{
	auto converter = D->findConverter(propertyType);
	if(!converter)// use fallback method
		return serializeValue(propertyType, value);
	else
		return converter->serializeCbor(propertyType, value, this);
}

QVariant QCborSerializer::deserializeVariant(int propertyType, const QCborValue &value, QObject *parent) const
{
	auto converter = D->findConverter(propertyType,
									  value.type(),
									  value.isTag() ? value.tag() : QCborTypeConverter::NoTag);
	QVariant variant;
	if(!converter)// use fallback method
		variant = deserializeValue(propertyType, value);
	else
		variant = converter->deserializeCbor(propertyType, value, parent, this);

	if(propertyType != QMetaType::UnknownType) {
		auto vType = variant.typeName();

		// exclude special values that can convert from null, but should not do so
		auto allowConvert = true;
		if(propertyType == QMetaType::QString && value.isNull())
			allowConvert = false;

		if(allowConvert && variant.canConvert(propertyType) && variant.convert(propertyType))
			return variant;
		else if(d->allowNull && value.isNull())
			return QVariant{propertyType, nullptr};
		else {
			throw QJsonDeserializationException(QByteArray("Failed to convert deserialized variant of type ") +
												(vType ? vType : "<unknown>") +
												QByteArray(" to property type ") +
												QMetaType::typeName(propertyType) +
												QByteArray(". Make shure to register converters with the QJsonSerializer::register* methods"));
		}
	} else
		return variant;
}

QCborValue QCborSerializer::serializeValue(int propertyType, const QVariant &value) const
{
	const auto cValue = QCborValue::fromVariant(value);
	if (!cValue.isTag()) {
		const auto mTag = typeTag(propertyType == QMetaType::UnknownType ? value.userType() : propertyType);
		if (mTag != QCborTypeConverter::NoTag)
			return {mTag, cValue};
	}
	return cValue;
}

QVariant QCborSerializer::deserializeValue(int propertyType, const QCborValue &value) const
{
	// perform strict validations
	if (d->validationFlags.testFlag(StrictBasicTypes)) {
		auto doThrow = false;
		switch (propertyType) {
		case QMetaType::Bool:
			if (value.isBool())
				doThrow = true;
			break;
		case QMetaType::Int:
		case QMetaType::UInt:
		case QMetaType::Long:
		case QMetaType::LongLong:
		case QMetaType::Short:
		case QMetaType::ULong:
		case QMetaType::ULongLong:
		case QMetaType::UShort:
		case QMetaType::SChar:
		case QMetaType::UChar:
			if (!value.isInteger())
				doThrow = true;
			break;
		case QMetaType::Float:
		case QMetaType::Double:
			if (value.isDouble())
				doThrow = true;
			break;
		case QMetaType::Char:
		case QMetaType::QChar:
		case QMetaType::QString:
		case QMetaType::QColor:  // TODO make tagged
		case QMetaType::QFont:  // TODO make tagged
			if (!value.isString())
				doThrow = true;
			break;
		case QMetaType::QByteArray:
			if (!value.isByteArray())
				doThrow = true;
			break;
		case QMetaType::Nullptr:
			if (value.isNull())
				doThrow = true;
			break;
		case QMetaType::QDate:
		case QMetaType::QTime:
		case QMetaType::QDateTime:
			if (!value.isDateTime() &&
				!(value.tag() == QCborKnownTags::DateTimeString && value.taggedValue().isString()) &&
				!(value.tag() == QCborKnownTags::UnixTime_t && value.taggedValue().isInteger()))
				doThrow = true;
			break;
		case QMetaType::QUrl:
			if (!value.isUrl() &&
				!(value.tag() == QCborKnownTags::Url && value.taggedValue().isString()))
				doThrow = true;
			break;
		case QMetaType::QUuid:
			if (!value.isUuid() &&
				!(value.tag() == QCborKnownTags::Uuid && value.taggedValue().isByteArray()))
				doThrow = true;
			break;
		case QMetaType::QRegularExpression:
			if (!value.isRegularExpression() &&
				!(value.tag() == QCborKnownTags::RegularExpression && value.taggedValue().isString()))
				doThrow = true;
			break;
		default:
			break;
		}

		if (const auto mTag = typeTag(propertyType);
			mTag != QCborTypeConverter::NoTag &&
			mTag != value.tag())
			doThrow = true;

		if (doThrow) {
			throw QJsonDeserializationException(QByteArray("Failed to deserialze CBOR-value to type ") +
												QMetaType::typeName(propertyType) +
												QByteArray("because the given CBOR-value failed strict validation"));
		}
	}

	return value.toVariant();
}

// ------------- private implementation -------------

QSharedPointer<QCborTypeConverter> QCborSerializerPrivate::findConverter(int propertyType, QCborValue::Type valueType, QCborTag tag)
{
	const auto isSerialization = valueType == QCborValue::Undefined;
	QReadLocker tLocker{&typeConverterLock};

	// first: check if already cached
	QSharedPointer<QCborTypeConverter> cachedConverter = nullptr; //in case json type did not match
	if (isSerialization)
		cachedConverter = typeConverterSerCache.value(propertyType).dynamicCast<QCborTypeConverter>();
	else {
		cachedConverter = typeConverterDeserCache.value(propertyType).dynamicCast<QCborTypeConverter>();
		if(cachedConverter && !cachedConverter->canDeserialize(valueType, tag))
			cachedConverter.clear();
	}
	if (cachedConverter)
		return cachedConverter;

	// second: check if the list of explicit converters has a matching one
	for (const auto &jConverter : qAsConst(typeConverters)) {
		if (const auto converter = jConverter.dynamicCast<QCborTypeConverter>();
			converter &&
			converter->canConvert(propertyType) &&
			(isSerialization || converter->canDeserialize(valueType, tag))) {
			// elevate lock
			tLocker.unlock();
			QWriteLocker wtLocker{&typeConverterLock};
			// add converter to cache
			if (isSerialization)
				typeConverterSerCache.insert(propertyType, converter);
			else
				typeConverterDeserCache.insert(propertyType, converter);
			return converter;
		}
	}

	// third: check in the list of global convert factories
	QReadLocker fLocker{&factoryLock};
	for (const auto &jFactory : qAsConst(typeConverterFactories)) {
		if (const auto factory = jFactory.dynamicCast<QCborTypeConverterFactory>();
			factory &&
			factory->canConvert(propertyType) &&
			(isSerialization || factory->canDeserialize(valueType, tag))) {
			auto converter = factory->createCborConverter();
			if (converter) {
				// elevate lock (keep unlocking order to prevent deadlocks)
				fLocker.unlock();
				tLocker.unlock();
				QWriteLocker wtLocker{&typeConverterLock};
				// add converter to list and cache
				typeConverters.append(converter);
				if (isSerialization)
					typeConverterSerCache.insert(propertyType, converter);
				else
					typeConverterDeserCache.insert(propertyType, converter);
				return converter;
			}
		}
	}

	// fourth: no converter found: return default converter
	return nullptr;
}
