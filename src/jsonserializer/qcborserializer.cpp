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

QVariant QCborSerializer::getProperty(const char *name) const
{
	return QJsonSerializer::getProperty(name);
}

QJsonValue QCborSerializer::serializeSubtype(QMetaProperty property, const QVariant &value) const
{
	return QJsonSerializer::serializeSubtype(property, value);
}

QJsonValue QCborSerializer::serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const
{
	return QJsonSerializer::serializeSubtype(propertyType, value, traceHint);
}

QVariant QCborSerializer::deserializeSubtype(QMetaProperty property, const QJsonValue &value, QObject *parent) const
{
	return QJsonSerializer::deserializeSubtype(property, value, parent);
}

QVariant QCborSerializer::deserializeSubtype(int propertyType, const QJsonValue &value, QObject *parent, const QByteArray &traceHint) const
{
	return QJsonSerializer::deserializeSubtype(propertyType, value, parent, traceHint);
}

QCborValue QCborSerializer::serializeVariant(int propertyType, const QVariant &value) const
{
	auto converter = D->findConverter(propertyType);
	if (converter)
		return converter->serializeCbor(propertyType, value, this);
	else // use fallback method
		return serializeValue(propertyType, value);
}

QVariant QCborSerializer::deserializeVariant(int propertyType, const QCborValue &value, QObject *parent) const
{
	auto converter = D->findConverter(propertyType,
									  value.type(),
									  value.isTag() ? value.tag() : QCborTypeConverter::NoTag);
	QVariant variant;
	if (converter)
		variant = converter->deserializeCbor(propertyType, value, parent, this);
	else // use fallback method
		variant = deserializeValue(propertyType, value);

	if(propertyType != QMetaType::UnknownType) {
		auto vType = variant.typeName();

		// exclude special values that can convert from null, but should not do so
		auto allowConvert = true;
		if ((propertyType == QMetaType::QString ||
			 propertyType == QMetaType::QByteArray)
			&& value.isNull())
			allowConvert = false;

		if (allowConvert && variant.canConvert(propertyType) && variant.convert(propertyType))
			return variant;
		else if (d->allowNull && value.isNull())
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
	if (propertyType == QMetaType::UnknownType)
		propertyType = value.userType();

	// handle special values
	QCborValue cValue;
	if (propertyType == QMetaType::QDate) // TODO move to own type converter
		cValue = QCborValue{QDateTime{value.toDate()}};
	else if (propertyType == QMetaType::QTime) {
		if (const auto cTime = value.toTime(); cTime.isValid())
			cValue = QCborValue{QDateTime{QDate{1970, 1, 1}, value.toTime()}};
		else
			cValue = QCborValue{QDateTime{}};
	} else if (typeTag(propertyType) == QCborKnownTags::UnixTime_t) {
		QDateTime uDt;
		if (propertyType == QMetaType::QDate)
			uDt = QDateTime{value.toDate()};
		else
			uDt = value.toDateTime();
		cValue = QCborValue{QCborKnownTags::UnixTime_t, uDt.toUTC().toSecsSinceEpoch()};
	} else
		cValue = QCborValue::fromVariant(value);

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

		QList<QCborTag> expectedTags;
		const auto testValue = value.isTag() ? value.taggedValue() : value;
		switch (propertyType) {
		case QMetaType::Bool:
			if (!testValue.isBool())
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
			if (!testValue.isInteger())
				doThrow = true;
			break;
		case QMetaType::Float:
		case QMetaType::Double:
			if (!testValue.isDouble())
				doThrow = true;
			break;
		case QMetaType::Char:
		case QMetaType::QChar:
		case QMetaType::QString:
			expectedTags = {
				QCborTypeConverter::NoTag,
				static_cast<QCborTag>(QCborKnownTags::Base64),
				static_cast<QCborTag>(QCborKnownTags::Base64url)
			};
			Q_FALLTHROUGH();
		case QMetaType::QColor:
		case QMetaType::QFont:
			if (!testValue.isString())
				doThrow = true;
			break;
		case QMetaType::QByteArray:
			expectedTags = {
				QCborTypeConverter::NoTag,
				static_cast<QCborTag>(QCborKnownTags::ExpectedBase64),
				static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url),
				static_cast<QCborTag>(QCborKnownTags::ExpectedBase16)
			};
			if (!testValue.isByteArray())
				doThrow = true;
			break;
		case QMetaType::Nullptr:
			if (!testValue.isNull())
				doThrow = true;
			break;
		case QMetaType::QDate:
		case QMetaType::QTime:
		case QMetaType::QDateTime:
			if (!testValue.isDateTime()) {
				if (testValue.isString())
					expectedTags = {static_cast<QCborTag>(QCborKnownTags::DateTimeString)};
				else if (testValue.isInteger())
					expectedTags = {static_cast<QCborTag>(QCborKnownTags::UnixTime_t)};
				else
					doThrow = true;
			}
			break;
		case QMetaType::QUrl:
			if (!value.isUrl()) {
				if (testValue.isString())
					expectedTags = {static_cast<QCborTag>(QCborKnownTags::Url)};
				else
					doThrow = true;
			}
			break;
		case QMetaType::QUuid:
			if (!value.isUuid()) {
				if (testValue.isByteArray())
					expectedTags = {static_cast<QCborTag>(QCborKnownTags::Uuid)};
				else
					doThrow = true;
			}
			break;
		case QMetaType::QRegularExpression:
			if (!value.isRegularExpression()) {
				if (testValue.isString())
					expectedTags = {static_cast<QCborTag>(QCborKnownTags::RegularExpression)};
				else
					doThrow = true;
			}
			break;
		default:
			break;
		}

		if (const auto mTag = typeTag(propertyType);
			mTag != QCborTypeConverter::NoTag &&
			mTag != value.tag())
			doThrow = true;
		else if (!expectedTags.isEmpty() &&
				 !expectedTags.contains(value.tag()))
			doThrow = true;

		if (doThrow) {
			throw QJsonDeserializationException(QByteArray("Failed to deserialze CBOR-value to type ") +
												QMetaType::typeName(propertyType) +
												QByteArray("because the given CBOR-value failed strict validation"));
		}
	}

	const auto res = value.toVariant();
	if (propertyType == QMetaType::QDate)
		return res.toDateTime().date();
	else if (propertyType == QMetaType::QTime)
		return res.toDateTime().time();
	else if (value.tag() == QCborKnownTags::UnixTime_t) {
		auto dt = QDateTime::fromSecsSinceEpoch(value.taggedValue().toInteger(), Qt::UTC);
		if (propertyType == QMetaType::QDate)
			return dt.date();
		else
			return dt;
	} else if (value.tag() == QCborKnownTags::Uuid && // WORKAROUND report and remove QTBUG-79196
			 res.userType() != QMetaType::QUuid)
		return QUuid::fromRfc4122(res.toByteArray());
	else if (value.tag() == QCborKnownTags::RegularExpression && // WORKAROUND report and remove QTBUG-79196
			 res.userType() != QMetaType::QRegularExpression)
		return QRegularExpression{res.toString()};
	else
		return res;
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
		if(cachedConverter && !cachedConverter->canDeserialize(tag, propertyType, valueType))
			cachedConverter.clear();
	}
	if (cachedConverter)
		return cachedConverter;

	// second: check if the list of explicit converters has a matching one
	for (const auto &jConverter : qAsConst(typeConverters)) {
		if (const auto converter = jConverter.dynamicCast<QCborTypeConverter>();
			converter &&
			(isSerialization ?
				converter->canConvert(propertyType) :
				converter->canDeserialize(tag, propertyType, valueType))) {
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
			(isSerialization ?
							 factory->canConvert(propertyType) :
							 factory->canDeserialize(tag, propertyType, valueType))) {
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
