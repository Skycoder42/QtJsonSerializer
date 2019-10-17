#include "qjsonserializerbase.h"
#include "qjsonserializerbase_p.h"
#include "qjsonexceptioncontext_p.h"

#include <optional>
#include <variant>
#include <cmath>

#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>

//#include "typeconverters/qjsonobjectconverter_p.h"
//#include "typeconverters/qjsongadgetconverter_p.h"
//#include "typeconverters/qjsonenumconverter_p.h"
//#include "typeconverters/qjsonmapconverter_p.h"
//#include "typeconverters/qjsonmultimapconverter_p.h"
//#include "typeconverters/qjsonlistconverter_p.h"
//#include "typeconverters/qjsonjsonconverter_p.h"
//#include "typeconverters/qjsonpairconverter_p.h"
//#include "typeconverters/qjsonbytearrayconverter_p.h"
//#include "typeconverters/qjsonversionnumberconverter_p.h"
//#include "typeconverters/qjsongeomconverter_p.h"
//#include "typeconverters/qjsonlocaleconverter_p.h"
//#include "typeconverters/qjsonregularexpressionconverter_p.h"
//#include "typeconverters/qjsonstdtupleconverter_p.h"
#include "typeconverters/qjsonchronodurationconverter_p.h"
//#include "typeconverters/qjsonstdoptionalconverter_p.h"
//#include "typeconverters/qjsonstdvariantconverter_p.h"

#ifndef NO_REGISTER_JSON_CONVERTERS
Q_COREAPP_STARTUP_FUNCTION(qtJsonSerializerRegisterTypes);
#else
void qtJsonSerializerRegisterTypes();
#endif

QJsonSerializerBase::QJsonSerializerBase(QObject *parent) :
	QJsonSerializerBase{*new QJsonSerializerBasePrivate{}, parent}
{}

QJsonSerializerBase::QJsonSerializerBase(QJsonSerializerBasePrivate &dd, QObject *parent) :
	QObject{dd, parent}
{}

bool QJsonSerializerBase::allowDefaultNull() const
{
	Q_D(const QJsonSerializerBase);
	return d->allowNull;
}

bool QJsonSerializerBase::keepObjectName() const
{
	Q_D(const QJsonSerializerBase);
	return d->keepObjectName;
}

bool QJsonSerializerBase::enumAsString() const
{
	Q_D(const QJsonSerializerBase);
	return d->enumAsString;
}

bool QJsonSerializerBase::useBcp47Locale() const
{
	Q_D(const QJsonSerializerBase);
	return d->useBcp47Locale;
}

QJsonSerializerBase::ValidationFlags QJsonSerializerBase::validationFlags() const
{
	Q_D(const QJsonSerializerBase);
	return d->validationFlags;
}

QJsonSerializerBase::Polymorphing QJsonSerializerBase::polymorphing() const
{
	Q_D(const QJsonSerializerBase);
	return d->polymorphing;
}

QJsonSerializerBase::MultiMapMode QJsonSerializerBase::multiMapMode() const
{
	Q_D(const QJsonSerializerBase);
	return d->multiMapMode;
}

bool QJsonSerializerBase::ignoresStoredAttribute() const
{
	Q_D(const QJsonSerializerBase);
	return d->ignoreStoredAttribute;
}

void QJsonSerializerBase::addJsonTypeConverterFactory(const QSharedPointer<QJsonTypeConverterFactory> &factory)
{
	QJsonSerializerBasePrivate::typeConverterFactories.insertSorted(factory);
}

void QJsonSerializerBase::addJsonTypeConverter(const QSharedPointer<QJsonTypeConverter> &converter)
{
	Q_D(QJsonSerializerBase);
	Q_ASSERT_X(converter, Q_FUNC_INFO, "converter must not be null!");
	d->typeConverters.insertSorted(converter);
	d->serCache.clear();
	d->deserCache.clear();
}

void QJsonSerializerBase::setAllowDefaultNull(bool allowDefaultNull)
{
	Q_D(QJsonSerializerBase);
	if(d->allowNull == allowDefaultNull)
		return;

	d->allowNull = allowDefaultNull;
	emit allowDefaultNullChanged(d->allowNull, {});
}

void QJsonSerializerBase::setKeepObjectName(bool keepObjectName)
{
	Q_D(QJsonSerializerBase);
	if(d->keepObjectName == keepObjectName)
		return;

	d->keepObjectName = keepObjectName;
	emit keepObjectNameChanged(d->keepObjectName, {});
}

void QJsonSerializerBase::setEnumAsString(bool enumAsString)
{
	Q_D(QJsonSerializerBase);
	if(d->enumAsString == enumAsString)
		return;

	d->enumAsString = enumAsString;
	emit enumAsStringChanged(d->enumAsString, {});
}

void QJsonSerializerBase::setUseBcp47Locale(bool useBcp47Locale)
{
	Q_D(QJsonSerializerBase);
	if(d->useBcp47Locale == useBcp47Locale)
		return;

	d->useBcp47Locale = useBcp47Locale;
	emit useBcp47LocaleChanged(d->useBcp47Locale, {});
}

void QJsonSerializerBase::setValidationFlags(ValidationFlags validationFlags)
{
	Q_D(QJsonSerializerBase);
	if(d->validationFlags == validationFlags)
		return;

	d->validationFlags = validationFlags;
	emit validationFlagsChanged(d->validationFlags, {});
}

void QJsonSerializerBase::setPolymorphing(QJsonSerializerBase::Polymorphing polymorphing)
{
	Q_D(QJsonSerializerBase);
	if(d->polymorphing == polymorphing)
		return;

	d->polymorphing = polymorphing;
	emit polymorphingChanged(d->polymorphing, {});
}

void QJsonSerializerBase::setMultiMapMode(QJsonSerializerBase::MultiMapMode multiMapMode)
{
	Q_D(QJsonSerializerBase);
	if(d->multiMapMode == multiMapMode)
		return;

	d->multiMapMode = multiMapMode;
	emit multiMapModeChanged(d->multiMapMode, {});
}

void QJsonSerializerBase::setIgnoreStoredAttribute(bool ignoreStoredAttribute)
{
	Q_D(QJsonSerializerBase);
	if(d->ignoreStoredAttribute == ignoreStoredAttribute)
		return;

	d->ignoreStoredAttribute = ignoreStoredAttribute;
	emit ignoreStoredAttributeChanged(d->ignoreStoredAttribute, {});
}

QVariant QJsonSerializerBase::getProperty(const char *name) const
{
	return property(name);
}

QCborValue QJsonSerializerBase::serializeSubtype(const QMetaProperty &property, const QVariant &value) const
{
	Q_D(const QJsonSerializerBase);
	QJsonExceptionContext ctx(property);
	if (property.isEnumType())
		return serializeVariant(d->getEnumId(property.enumerator(), true), value);
	else
		return serializeVariant(property.userType(), value);
}

QCborValue QJsonSerializerBase::serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx(propertyType, traceHint);
	return serializeVariant(propertyType, value);
}

QVariant QJsonSerializerBase::deserializeSubtype(const QMetaProperty &property, const QCborValue &value, QObject *parent) const
{
	Q_D(const QJsonSerializerBase);
	QJsonExceptionContext ctx(property);
	if(property.isEnumType())
		return deserializeVariant(d->getEnumId(property.enumerator(), false), value, parent);
	else
		return deserializeVariant(property.userType(), value, parent);
}

QVariant QJsonSerializerBase::deserializeSubtype(int propertyType, const QCborValue &value, QObject *parent, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx(propertyType, traceHint);
	return deserializeVariant(propertyType, value, parent);
}

QCborValue QJsonSerializerBase::serializeVariant(int propertyType, const QVariant &value) const
{
	Q_D(const QJsonSerializerBase);
	// first: find a converter and convert to cbor
	auto converter = d->findSerConverter(propertyType);
	QCborValue res;
	if (converter)
		res = converter->serialize(propertyType, value, this);
	else
		res = d->serializeValue(propertyType, value);

	// second: check if an override tag is given, and if yes, override the normal tag
	if (const auto mTag = typeTag(propertyType); mTag != QJsonTypeConverter::NoTag)
		return {mTag, res.isTag() ? res.taggedValue() : res};
	else
		return res;
}

QVariant QJsonSerializerBase::deserializeVariant(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_D(const QJsonSerializerBase);
	// first: find a converter and convert the data to QVariant
	auto converter = d->findDeserConverter(propertyType,
										   value.isTag() ? value.tag() : QJsonTypeConverter::NoTag,
										   value.isTag() ? value.taggedValue().type() : value.type());

	QVariant variant;
	if (converter) {
		if (jsonMode())
			variant = converter->deserializeJson(propertyType, value, parent, this);
		else
			variant = converter->deserializeCbor(propertyType, value, parent, this);
	} else {
		if (jsonMode())
			variant = d->deserializeJsonValue(propertyType, value);
		else
			variant = d->deserializeCborValue(propertyType, value);
	}

	// second: if the type was given, enforce a conversion to that type
	if(propertyType != QMetaType::UnknownType) {
		auto vType = variant.typeName();

		// exclude special values that can convert from null, but should not do so
		auto allowConvert = true;
		switch (propertyType) {
		case QMetaType::QString:
		case QMetaType::QByteArray:
			if (value.isNull())
				allowConvert = false;
			break;
		default:
			break;
		}

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

void QJsonSerializerBase::registerInverseTypedefImpl(int typeId, const char *normalizedTypeName)
{
	QWriteLocker lock{&QJsonSerializerBasePrivate::typedefLock};
	QJsonSerializerBasePrivate::typedefMapping.insert(typeId, normalizedTypeName);
}



QReadWriteLock QJsonSerializerBasePrivate::typedefLock;
QHash<int, QByteArray> QJsonSerializerBasePrivate::typedefMapping;
QJsonSerializerBasePrivate::ConverterStore<QJsonTypeConverterFactory> QJsonSerializerBasePrivate::typeConverterFactories {
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonObjectConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonGadgetConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonEnumConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonMapConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonMultiMapConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonListConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonJsonValueConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonJsonObjectConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonJsonArrayConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonPairConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonBytearrayConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonVersionNumberConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonSizeConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonPointConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonLineConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonRectConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonLocaleConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonRegularExpressionConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonStdTupleConverter>>::create(),
	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonChronoDurationConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonStdOptionalConverter>>::create(),
//	QSharedPointer<QJsonTypeConverterStandardFactory<QJsonStdVariantConverter>>::create()
};

QByteArray QJsonSerializerBasePrivate::getTypeName(int propertyType)
{
	QReadLocker lock{&typedefLock};
	return typedefMapping.value(propertyType, QMetaType::typeName(propertyType));
}

QSharedPointer<QJsonTypeConverter> QJsonSerializerBasePrivate::findSerConverter(int propertyType) const
{
	// first: check if already cached
	if (auto converter = serCache.get(propertyType); converter)
		return converter;

	// second: check if the list of explicit converters has a matching one
	QReadLocker cLocker{&typeConverters.lock};
	for (const auto &converter : qAsConst(typeConverters.store)) {
		if (converter && converter->canConvert(propertyType)) {
			// add converter to cache
			serCache.add(propertyType, converter);
			return converter;
		}
	}
	cLocker.unlock();

	// third: check in the list of global convert factories
	QReadLocker fLocker{&typeConverterFactories.lock};
	for (const auto &factory : qAsConst(typeConverterFactories.store)) {
		if (factory && factory->canConvert(propertyType)) {
			auto converter = factory->createConverter();
			if (converter) {
				// add converter to list and cache
				typeConverters.insertSorted(converter);
				serCache.add(propertyType, converter);
				return converter;
			}
		}
	}
	fLocker.unlock();

	// fourth: no converter found: return default converter
	return nullptr;
}

QSharedPointer<QJsonTypeConverter> QJsonSerializerBasePrivate::findDeserConverter(int &propertyType, QCborTag tag, QCborValue::Type type) const
{
	Q_Q(const QJsonSerializerBase);

	// zeroth: if no property type is given, try out any types associated with the tag
	if (propertyType == QMetaType::UnknownType && tag != QJsonTypeConverter::NoTag) {
		const auto tList = q->typesForTag(tag);
		for (auto typeId : tList) {
			// if any of those types has a working converter, just use that one
			auto res = findDeserConverter(typeId, tag, type);
			if (res) {
				propertyType = typeId;
				return res;
			}
		}
	}

	// first: check if already cached
	if (auto converter = deserCache.get(propertyType);
		converter && converter->canDeserialize(propertyType, tag, type, q) > 0)
		return converter;

	// second: check if the list of explicit converters has a matching one
	QReadLocker cLocker{&typeConverters.lock};
	auto throwWrongTag = false;
	std::optional<std::pair<std::variant<QSharedPointer<QJsonTypeConverter>,
										 QSharedPointer<QJsonTypeConverterFactory>>,
							int>> guessConverter;
	for (const auto &converter : qAsConst(typeConverters.store)) {
		if (converter) {
			auto testType = propertyType;
			switch (converter->canDeserialize(testType, tag, type, q)) {
			case QJsonTypeConverter::Negative:
				continue;
			case QJsonTypeConverter::WrongTag:
				throwWrongTag = true;
				continue;
			case QJsonTypeConverter::Guessed:
				if (!guessConverter)
					guessConverter = std::make_pair(converter, testType);
				continue;
			case QJsonTypeConverter::Positive:
				break;
			}

			// add converter to cache (only happens for positive cases)
			deserCache.add(propertyType, converter);
			return converter;
		}
	}
	cLocker.unlock();

	// third: check in the list of global convert factories
	QReadLocker fLocker{&typeConverterFactories.lock};
	for (const auto &factory : qAsConst(typeConverterFactories.store)) {
		if (factory) {
			auto testType = propertyType;
			switch (factory->canDeserialize(testType, tag, type, q)) {
			case QJsonTypeConverter::Negative:
				continue;
			case QJsonTypeConverter::WrongTag:
				throwWrongTag = true;
				continue;
			case QJsonTypeConverter::Guessed:
				if (!guessConverter)
					guessConverter = std::make_pair(factory, testType);
				continue;
			case QJsonTypeConverter::Positive:
				break;
			}

			auto converter = factory->createConverter();
			if (converter) {
				// add converter to list and cache (keep unlocking order to prevent deadlocks)
				typeConverters.insertSorted(converter);
				deserCache.add(propertyType, converter);
				return converter;
			}
		}
	}
	fLocker.unlock();

	// fourth: if a guessed converter is available, use that one
	if (guessConverter) {
		// extract converter from info;
		auto &[confInfo, newType] = *guessConverter;
		const auto isFactory = std::holds_alternative<QSharedPointer<QJsonTypeConverterFactory>>(confInfo);
		QSharedPointer<QJsonTypeConverter> converter;
		if (isFactory)
			converter = std::get<QSharedPointer<QJsonTypeConverterFactory>>(confInfo)->createConverter();
		else
			converter = std::get<QSharedPointer<QJsonTypeConverter>>(confInfo);

		// if valid, add to cache, set the type and return
		if (converter) {
			// add converter to list and cache
			if (isFactory)
				typeConverters.insertSorted(converter);
			deserCache.add(propertyType, converter);
			propertyType = newType;
			return converter;
		}
	}

	// fifth: if a wrong tag mark was set, throw an expection
	if (throwWrongTag) {
		throw QJsonDeserializationException{QByteArray{"Found converter able to handle data of type "} +
											QMetaType::typeName(propertyType) +
											", but the given CBOR tag " +
											QByteArray::number(static_cast<quint64>(tag)) +
											" is not convertible to that type."};
	}

	// sixth: no converter found: return default converter
	return nullptr;
}

int QJsonSerializerBasePrivate::getEnumId(QMetaEnum metaEnum, bool ser) const
{
	QByteArray eName = metaEnum.name();
	if (const QByteArray scope = metaEnum.scope(); !scope.isEmpty())
		eName = scope + "::" + eName;
	const auto eTypeId = QMetaType::type(eName);
	if (eTypeId == QMetaType::UnknownType) {
		if (ser)
			throw QJsonSerializationException{"Unable to determine typeid of meta enum " + eName};
		else
			throw QJsonDeserializationException{"Unable to determine typeid of meta enum " + eName};
	} else
		return eTypeId;
}

QCborValue QJsonSerializerBasePrivate::serializeValue(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	return QCborValue::fromVariant(value);
}

QVariant QJsonSerializerBasePrivate::deserializeCborValue(int propertyType, const QCborValue &value) const
{
	Q_Q(const QJsonSerializerBase);
	// perform strict validations
	if (validationFlags.testFlag(ValidationFlag::StrictBasicTypes)) {
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
				QJsonTypeConverter::NoTag,
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
				QJsonTypeConverter::NoTag,
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

		if (const auto mTag = q->typeTag(propertyType);
			mTag != QJsonTypeConverter::NoTag &&
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
	// WORKAROUND for QTBUG-79196
	if (value.tag() == QCborKnownTags::Uuid &&
		res.userType() != QMetaType::QUuid)
		return QUuid::fromRfc4122(res.toByteArray());
	else if (value.tag() == QCborKnownTags::RegularExpression &&
			 res.userType() != QMetaType::QRegularExpression)
		return QRegularExpression{res.toString()};
	else
		return res;
}

QVariant QJsonSerializerBasePrivate::deserializeJsonValue(int propertyType, const QCborValue &value) const
{
	// perform strict validations
	if (validationFlags.testFlag(ValidationFlag::StrictBasicTypes)) {
		auto doThrow = false;
		switch (propertyType) {
		case QMetaType::Bool:
			if (!value.isBool())
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
			if (value.isDouble()) {
				if (auto val = value.toDouble(); trunc(val) != val)
					doThrow = true;
			} else if (!value.isInteger())
				doThrow = true;
			break;
		case QMetaType::QChar:
		case QMetaType::QString:
		case QMetaType::Char:
		case QMetaType::QColor:
		case QMetaType::QUrl:
		case QMetaType::QFont:
		case QMetaType::QUuid:
			if (!value.isString())
				doThrow = true;
			break;
		case QMetaType::Nullptr:
			if (!value.isNull())
				doThrow = true;
			break;
		case QMetaType::Float:
		case QMetaType::Double:
			if (!value.isDouble())
				doThrow = true;
			break;
		default:
			break;
		}

		if (doThrow) {
			throw QJsonDeserializationException(QByteArray("Failed to deserialze JSON-value to type ") +
												QMetaType::typeName(propertyType) +
												QByteArray("because the given JSON-value failed strict validation"));
		}
	}

	return value.toVariant();
}
