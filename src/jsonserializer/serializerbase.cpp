#include "serializerbase.h"
#include "serializerbase_p.h"
#include "exceptioncontext_p.h"

#include <optional>
#include <variant>
#include <cmath>

#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QScopeGuard>

#include "typeconverters/bitarrayconverter_p.h"
#include "typeconverters/bytearrayconverter_p.h"
#include "typeconverters/cborconverter_p.h"
#include "typeconverters/datetimeconverter_p.h"
#include "typeconverters/enumconverter_p.h"
#include "typeconverters/gadgetconverter_p.h"
#include "typeconverters/geomconverter_p.h"
#include "typeconverters/legacygeomconverter_p.h"
#include "typeconverters/listconverter_p.h"
#include "typeconverters/localeconverter_p.h"
#include "typeconverters/mapconverter_p.h"
#include "typeconverters/multimapconverter_p.h"
#include "typeconverters/objectconverter_p.h"
#include "typeconverters/pairconverter_p.h"
#include "typeconverters/smartpointerconverter_p.h"
#include "typeconverters/stdchronodurationconverter_p.h"
#include "typeconverters/stdoptionalconverter_p.h"
#include "typeconverters/stdtupleconverter_p.h"
#include "typeconverters/stdvariantconverter_p.h"
#include "typeconverters/versionnumberconverter_p.h"
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

#ifndef NO_REGISTER_JSON_CONVERTERS
namespace {
void qtJsonSerializerRegisterTypes() {
	QtJsonSerializer::registerTypes();
}
}
Q_COREAPP_STARTUP_FUNCTION(qtJsonSerializerRegisterTypes);
#endif

Q_LOGGING_CATEGORY(QtJsonSerializer::logSerializer, "qt.jsonserializer.serializer")
Q_LOGGING_CATEGORY(QtJsonSerializer::logSerializerExtractor, "qt.jsonserializer.serializer.extractor")

namespace {

class LogTag {
public:
	inline LogTag(QCborTag tag) : tag{tag} {}
	inline operator QCborTag() const { return tag; }
private:
	QCborTag tag;
};

QDebug operator<<(QDebug debug, LogTag tag) {
	if (tag != TypeConverter::NoTag)
		debug << ", CBOR-Tag" << static_cast<QCborTag>(tag);
	return debug;
}

}

SerializerBase::SerializerBase(QObject *parent) :
	SerializerBase{*new SerializerBasePrivate{}, parent}
{}

SerializerBase::SerializerBase(SerializerBasePrivate &dd, QObject *parent) :
	QObject{dd, parent}
{}

void SerializerBase::registerExtractor(int metaTypeId, const QSharedPointer<TypeExtractor> &extractor)
{
	SerializerBasePrivate::extractors.add(metaTypeId, extractor);
	qCDebug(logSerializerExtractor) << "Added extractor for type:" << QMetaType::typeName(metaTypeId);
}

bool SerializerBase::allowDefaultNull() const
{
	Q_D(const SerializerBase);
	return d->allowNull;
}

bool SerializerBase::keepObjectName() const
{
	Q_D(const SerializerBase);
	return d->keepObjectName;
}

bool SerializerBase::enumAsString() const
{
	Q_D(const SerializerBase);
	return d->enumAsString;
}

bool SerializerBase::versionAsString() const
{
	Q_D(const SerializerBase);
	return d->versionAsString;
}

bool SerializerBase::dateAsTimeStamp() const
{
	Q_D(const SerializerBase);
	return d->dateAsTimeStamp;
}

bool SerializerBase::useBcp47Locale() const
{
	Q_D(const SerializerBase);
	return d->useBcp47Locale;
}

SerializerBase::ValidationFlags SerializerBase::validationFlags() const
{
	Q_D(const SerializerBase);
	return d->validationFlags;
}

SerializerBase::Polymorphing SerializerBase::polymorphing() const
{
	Q_D(const SerializerBase);
	return d->polymorphing;
}

SerializerBase::MultiMapMode SerializerBase::multiMapMode() const
{
	Q_D(const SerializerBase);
	return d->multiMapMode;
}

bool SerializerBase::ignoresStoredAttribute() const
{
	Q_D(const SerializerBase);
	return d->ignoreStoredAttribute;
}

void SerializerBase::addJsonTypeConverterFactory(TypeConverterFactory *factory)
{
	QWriteLocker _{&SerializerBasePrivate::typeConverterFactoryLock};
	SerializerBasePrivate::typeConverterFactories.append(factory);
	qCDebug(logSerializer) << "Added new global converter factory:" << factory;
}

void SerializerBase::addJsonTypeConverter(const QSharedPointer<TypeConverter> &converter)
{
	Q_D(SerializerBase);
	Q_ASSERT_X(converter, Q_FUNC_INFO, "converter must not be null!");
	converter->setHelper(this);
	d->typeConverters.insertSorted(converter);
	d->serCache.clear();
	d->deserCache.clear();
	qCDebug(logSerializer) << "Added new local converter:" << converter->name();
}

void SerializerBase::setAllowDefaultNull(bool allowDefaultNull)
{
	Q_D(SerializerBase);
	if(d->allowNull == allowDefaultNull)
		return;

	d->allowNull = allowDefaultNull;
	emit allowDefaultNullChanged(d->allowNull, {});
}

void SerializerBase::setKeepObjectName(bool keepObjectName)
{
	Q_D(SerializerBase);
	if(d->keepObjectName == keepObjectName)
		return;

	d->keepObjectName = keepObjectName;
	emit keepObjectNameChanged(d->keepObjectName, {});
}

void SerializerBase::setEnumAsString(bool enumAsString)
{
	Q_D(SerializerBase);
	if(d->enumAsString == enumAsString)
		return;

	d->enumAsString = enumAsString;
	emit enumAsStringChanged(d->enumAsString, {});
}

void SerializerBase::setVersionAsString(bool versionAsString)
{
	Q_D(SerializerBase);
	if(d->versionAsString == versionAsString)
		return;

	d->versionAsString = versionAsString;
	emit versionAsStringChanged(d->versionAsString, {});
}

void SerializerBase::setDateAsTimeStamp(bool dateAsTimeStamp)
{
	Q_D(SerializerBase);
	if(d->dateAsTimeStamp == dateAsTimeStamp)
		return;

	d->dateAsTimeStamp = dateAsTimeStamp;
	emit dateAsTimeStampChanged(d->dateAsTimeStamp, {});
}

void SerializerBase::setUseBcp47Locale(bool useBcp47Locale)
{
	Q_D(SerializerBase);
	if(d->useBcp47Locale == useBcp47Locale)
		return;

	d->useBcp47Locale = useBcp47Locale;
	emit useBcp47LocaleChanged(d->useBcp47Locale, {});
}

void SerializerBase::setValidationFlags(ValidationFlags validationFlags)
{
	Q_D(SerializerBase);
	if(d->validationFlags == validationFlags)
		return;

	d->validationFlags = validationFlags;
	emit validationFlagsChanged(d->validationFlags, {});
}

void SerializerBase::setPolymorphing(SerializerBase::Polymorphing polymorphing)
{
	Q_D(SerializerBase);
	if(d->polymorphing == polymorphing)
		return;

	d->polymorphing = polymorphing;
	emit polymorphingChanged(d->polymorphing, {});
}

void SerializerBase::setMultiMapMode(SerializerBase::MultiMapMode multiMapMode)
{
	Q_D(SerializerBase);
	if(d->multiMapMode == multiMapMode)
		return;

	d->multiMapMode = multiMapMode;
	emit multiMapModeChanged(d->multiMapMode, {});
}

void SerializerBase::setIgnoreStoredAttribute(bool ignoreStoredAttribute)
{
	Q_D(SerializerBase);
	if(d->ignoreStoredAttribute == ignoreStoredAttribute)
		return;

	d->ignoreStoredAttribute = ignoreStoredAttribute;
	emit ignoreStoredAttributeChanged(d->ignoreStoredAttribute, {});
}

QVariant SerializerBase::getProperty(const char *name) const
{
	return property(name);
}

QSharedPointer<const TypeExtractor> SerializerBase::extractor(int metaTypeId) const
{
	const auto extractor = SerializerBasePrivate::extractors.get(metaTypeId);
	if (extractor)
		qCDebug(logSerializerExtractor) << "Found extractor for type:" << QMetaType::typeName(metaTypeId);
	else
		qCDebug(logSerializerExtractor) << "Unable to find extractor for type:" << QMetaType::typeName(metaTypeId);
	return extractor;
}

QCborValue SerializerBase::serializeSubtype(const QMetaProperty &property, const QVariant &value) const
{
	Q_D(const SerializerBase);
	ExceptionContext ctx(property);
	auto logGuard = qScopeGuard([](){
		qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
							   << "done";
	});
	if (property.isEnumType()) {
		const auto enumId = d->getEnumId(property.enumerator(), true);
		qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
							   << "Serializing subtype property" << property.name()
							   << "of enum type" << QMetaType::typeName(enumId);
		return serializeVariant(enumId, value);
	} else {
		qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
							   << "Serializing subtype property" << property.name()
							   << "of type" << QMetaType::typeName(property.userType());
		return serializeVariant(property.userType(), value);
	}
}

QCborValue SerializerBase::serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const
{
	ExceptionContext ctx(propertyType, traceHint);
	auto logGuard = qScopeGuard([](){
		qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
							   << "done";
	});
	qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
						   << "Serializing subtype property" << traceHint
						   << "of type" << QMetaType::typeName(propertyType);
	return serializeVariant(propertyType, value);
}

QVariant SerializerBase::deserializeSubtype(const QMetaProperty &property, const QCborValue &value, QObject *parent) const
{
	Q_D(const SerializerBase);
	ExceptionContext ctx(property);
	auto logGuard = qScopeGuard([](){
		qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
							   << "done";
	});
	if (property.isEnumType()) {
		const auto enumId = d->getEnumId(property.enumerator(), false);
		qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
							   << "Deserializing subtype property" << property.name()
							   << "of enum type" << QMetaType::typeName(enumId);
		return deserializeVariant(enumId, value, parent, true);
	} else {
		qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
							   << "Deserializing subtype property" << property.name()
							   << "of type" << QMetaType::typeName(property.userType());
		return deserializeVariant(property.userType(), value, parent);
	}
}

QVariant SerializerBase::deserializeSubtype(int propertyType, const QCborValue &value, QObject *parent, const QByteArray &traceHint) const
{
	ExceptionContext ctx(propertyType, traceHint);
	auto logGuard = qScopeGuard([](){
		qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
							   << "done";
	});
	qCDebug(logSerializer) << QByteArray{">"}.repeated(ExceptionContext::currentDepth()).constData()
						   << "Deserializing subtype property" << traceHint
						   << "of type" << QMetaType::typeName(propertyType);
	return deserializeVariant(propertyType, value, parent);
}

QCborValue SerializerBase::serializeVariant(int propertyType, const QVariant &value) const
{
	Q_D(const SerializerBase);
	// first: find a converter and convert to cbor
	auto converter = d->findSerConverter(propertyType);
	QCborValue res;
	if (converter)
		res = converter->serialize(propertyType, value);
	else
		res = d->serializeValue(propertyType, value);

	// second: check if an override tag is given, and if yes, override the normal tag
	if (const auto mTag = typeTag(propertyType); mTag != TypeConverter::NoTag)
		return {mTag, res.isTag() ? res.taggedValue() : res};
	else
		return res;
}

QVariant SerializerBase::deserializeVariant(int propertyType, const QCborValue &value, QObject *parent, bool skipConversion) const
{
	Q_D(const SerializerBase);
	// first: find a converter and convert the data to QVariant
	auto converter = d->findDeserConverter(propertyType,
										   value.isTag() ? value.tag() : TypeConverter::NoTag,
										   value.isTag() ? value.taggedValue().type() : value.type());

	QVariant variant;
	if (converter) {
		if (jsonMode())
			variant = converter->deserializeJson(propertyType, value, parent);
		else
			variant = converter->deserializeCbor(propertyType, value, parent);
	} else {
		if (jsonMode())
			variant = d->deserializeJsonValue(propertyType, value);
		else
			variant = d->deserializeCborValue(propertyType, value);
	}

	// second: if the type was given, enforce a conversion to that type (expect if skipped)
	if(!skipConversion && propertyType != QMetaType::UnknownType) {
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
            return QVariant{static_cast<QMetaType>(propertyType), nullptr};
		else {
			throw DeserializationException(QByteArray("Failed to convert deserialized variant of type ") +
										   (vType ? vType : "<unknown>") +
										   QByteArray(" to property type ") +
										   QMetaType::typeName(propertyType) +
										   QByteArray(". Make shure to register converters with the QJsonSerializer::register* methods"));
		}
	} else
		return variant;
}

// ------------- private implementation -------------

SerializerBasePrivate::ThreadSafeStore<TypeExtractor> SerializerBasePrivate::extractors;
QReadWriteLock SerializerBasePrivate::typeConverterFactoryLock;
QList<TypeConverterFactory*> SerializerBasePrivate::typeConverterFactories {
	new TypeConverterStandardFactory<BitArrayConverter>{},
	new TypeConverterStandardFactory<BytearrayConverter>{},
	new TypeConverterStandardFactory<CborConverter>{},
	new TypeConverterStandardFactory<DateTimeConverter>{},
	new TypeConverterStandardFactory<EnumConverter>{},
	new TypeConverterStandardFactory<GadgetConverter>{},
	new TypeConverterStandardFactory<GeomConverter>{},
	new TypeConverterStandardFactory<ListConverter>{},
	new TypeConverterStandardFactory<LocaleConverter>{},
	new TypeConverterStandardFactory<MapConverter>{},
	new TypeConverterStandardFactory<MultiMapConverter>{},
	new TypeConverterStandardFactory<ObjectConverter>{},
	new TypeConverterStandardFactory<PairConverter>{},
	new TypeConverterStandardFactory<SmartPointerConverter>{},
	new TypeConverterStandardFactory<StdChronoDurationConverter>{},
	new TypeConverterStandardFactory<StdOptionalConverter>{},
	new TypeConverterStandardFactory<StdTupleConverter>{},
	new TypeConverterStandardFactory<StdVariantConverter>{},
	new TypeConverterStandardFactory<VersionNumberConverter>{},

	new TypeConverterStandardFactory<LegacyGeomConverter>{}
};

QSharedPointer<TypeConverter> SerializerBasePrivate::findSerConverter(int propertyType) const
{
	// first: update converters from factories
	updateConverterStore();

	// second: check if already cached
	if (auto converter = serCache.get(propertyType); converter) {
		qCDebug(logSerializer) << "Found cached serialization converter" << converter->name()
							   << "for type:" <<  QMetaType::typeName(propertyType);
		return converter;
	}

	// third: check if the list of explicit converters has a matching one
	QReadLocker cLocker{&typeConverters.lock};
	for (const auto &converter : qAsConst(typeConverters.store)) {
		if (converter && converter->canConvert(propertyType)) {
			qCDebug(logSerializer) << "Found and cached serialization converter" << converter->name()
								   << "for type:" <<  QMetaType::typeName(propertyType);
			// add converter to cache and return it
			serCache.add(propertyType, converter);
			return converter;
		}
	}

	// fourth: no converter found: return default converter
	qCDebug(logSerializer) << "Unable to find serialization converte for type:" <<  QMetaType::typeName(propertyType)
						   << "- falling back to default QVariant to CBOR conversion";
	return nullptr;
}

QSharedPointer<TypeConverter> SerializerBasePrivate::findDeserConverter(int &propertyType, QCborTag tag, QCborValue::Type type) const
{
	Q_Q(const SerializerBase);
	// first: update converters from factories
	updateConverterStore();

	// second: if no property type is given, try out any types associated with the tag
	if (propertyType == QMetaType::UnknownType && tag != TypeConverter::NoTag) {
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

	// third: check if already cached
	if (auto converter = deserCache.get(propertyType);
		converter && converter->canDeserialize(propertyType, tag, type) > 0) {
		qCDebug(logSerializer) << "Found cached deserialization converter" << converter->name()
							   << "for type" <<  QMetaType::typeName(propertyType)
							   << LogTag{tag}
							   << "and CBOR-type" << type;
		return converter;
	}

	// fourth: check if the list of explicit converters has a matching one
	QReadLocker cLocker{&typeConverters.lock};
	auto throwWrongTag = false;
	std::optional<std::pair<QSharedPointer<TypeConverter>, int>> guessConverter;
	for (const auto &converter : qAsConst(typeConverters.store)) {
		if (converter) {
			auto testType = propertyType;
			switch (converter->canDeserialize(testType, tag, type)) {
			case TypeConverter::Negative:
				continue;
			case TypeConverter::WrongTag:
				throwWrongTag = true;
				continue;
			case TypeConverter::Guessed:
				if (!guessConverter)
					guessConverter = std::make_pair(converter, testType);
				continue;
			case TypeConverter::Positive:
				break;
			}

			// add converter to cache (only happens for positive cases)
			deserCache.add(propertyType, converter);
			qCDebug(logSerializer) << "Found and cached deserialization converter" << converter->name()
								   << "for type" <<  QMetaType::typeName(propertyType)
								   << LogTag{tag}
								   << "and CBOR-type" << type;
			return converter;
		}
	}
	cLocker.unlock();

	// fifth: if a guessed converter is available, use that one
	if (guessConverter) {
		// extract converter from info;
		auto &[converter, newType] = *guessConverter;
		// if valid, add to cache, set the type and return
		if (converter) {
			// add converter to list and cache
			propertyType = newType;
			deserCache.add(propertyType, converter);
			qCDebug(logSerializer) << "Found and cached deserialization converter" << converter->name()
								   << "by guessing the data with CBOR-tag" << tag
								   << "and CBOR-type" << type
								   << "is of type" << QMetaType::typeName(propertyType);
			return converter;
		}
	}

	// sixth: if a wrong tag mark was set, throw an expection
	if (throwWrongTag) {
		throw DeserializationException{QByteArray{"Found converter able to handle data of type "} +
									   QMetaType::typeName(propertyType) +
									   ", but the given CBOR tag " +
									   QByteArray::number(static_cast<quint64>(tag)) +
									   " is not convertible to that type."};
	}

	// seventh: no converter found: return default converter
	qCDebug(logSerializer) << "Unable to find deserialization converte for type" <<  QMetaType::typeName(propertyType)
						   << LogTag{tag}
						   << "and CBOR-type" << type
						   << "- falling back to default CBOR to QVariant conversion";
	return nullptr;
}

void SerializerBasePrivate::updateConverterStore() const
{
	Q_Q(const SerializerBase);
	QReadLocker fLocker{&typeConverterFactoryLock};
	if (typeConverterFactories.size() > typeConverters.factoryOffset.loadAcquire()) {
		QWriteLocker cLocker{&typeConverters.lock};
        for (int i = typeConverters.factoryOffset.loadAcquire(), max = typeConverterFactories.size(); i < max; ++i) {
			auto converter = typeConverterFactories[i]->createConverter();
			if (converter) {
				converter->setHelper(q);
				typeConverters.insertSorted(converter, cLocker);
				serCache.clear();
				deserCache.clear();
				qCDebug(logSerializer) << "Found and added new global converter:" << converter->name();
			}
		}
		typeConverters.factoryOffset.storeRelease(typeConverterFactories.size());
	}
}

int SerializerBasePrivate::getEnumId(QMetaEnum metaEnum, bool ser) const
{
	QByteArray eName = metaEnum.name();
	if (const QByteArray scope = metaEnum.scope(); !scope.isEmpty())
		eName = scope + "::" + eName;
	const auto eTypeId = QMetaType::type(eName);
	if (eTypeId == QMetaType::UnknownType) {
		if (ser)
			throw SerializationException{"Unable to determine typeid of meta enum " + eName};
		else
			throw DeserializationException{"Unable to determine typeid of meta enum " + eName};
	} else
		return eTypeId;
}

QCborValue SerializerBasePrivate::serializeValue(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	return QCborValue::fromVariant(value);
}

QVariant SerializerBasePrivate::deserializeCborValue(int propertyType, const QCborValue &value) const
{
	Q_Q(const SerializerBase);
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
				TypeConverter::NoTag,
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
				TypeConverter::NoTag,
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
			mTag != TypeConverter::NoTag &&
			mTag != value.tag())
			doThrow = true;
		else if (!expectedTags.isEmpty() &&
				 !expectedTags.contains(value.tag()))
			doThrow = true;

		if (doThrow) {
			throw DeserializationException(QByteArray("Failed to deserialze CBOR-value to type ") +
										   QMetaType::typeName(propertyType) +
										   QByteArray(" because the given CBOR-value failed strict validation"));
		}
	}

	return value.toVariant();
}

QVariant SerializerBasePrivate::deserializeJsonValue(int propertyType, const QCborValue &value) const
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
			throw DeserializationException(QByteArray("Failed to deserialze JSON-value to type ") +
										   QMetaType::typeName(propertyType) +
										   QByteArray("because the given JSON-value failed strict validation"));
		}
	}

	// special case: QRegularExpression, is missing a converter (and cannot be registered)
	if (propertyType == QMetaType::QRegularExpression &&
		value.isString())
		return QRegularExpression{value.toString()};
	else
		return value.toVariant();
}
