#include "qjsonserializer.h"
#include "qjsonserializer_p.h"
#include "qjsonexceptioncontext_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QUrl>
#include <QtCore/QVersionNumber>
#include <QtCore/QBuffer>
#include <QtCore/QLine>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QSize>

#include "typeconverters/qjsonobjectconverter_p.h"
#include "typeconverters/qjsongadgetconverter_p.h"
#include "typeconverters/qjsonmapconverter_p.h"
#include "typeconverters/qjsonlistconverter_p.h"
#include "typeconverters/qjsonjsonconverter_p.h"
#include "typeconverters/qjsonpairconverter_p.h"
#include "typeconverters/qjsonbytearrayconverter_p.h"
#include "typeconverters/qjsonversionnumberconverter_p.h"
#include "typeconverters/qjsongeomconverter_p.h"

static void qJsonSerializerStartup();
Q_COREAPP_STARTUP_FUNCTION(qJsonSerializerStartup)

QJsonSerializer::QJsonSerializer(QObject *parent) :
	QObject(parent),
	d(new QJsonSerializerPrivate())
{
	addJsonTypeConverter(new QJsonObjectConverter());
	addJsonTypeConverter(new QJsonGadgetConverter());
	addJsonTypeConverter(new QJsonMapConverter());
	addJsonTypeConverter(new QJsonListConverter());
	addJsonTypeConverter(new QJsonJsonValueConverter());
	addJsonTypeConverter(new QJsonJsonObjectConverter());
	addJsonTypeConverter(new QJsonJsonArrayConverter());
	addJsonTypeConverter(new QJsonPairConverter());
	addJsonTypeConverter(new QJsonBytearrayConverter());
	addJsonTypeConverter(new QJsonVersionNumberConverter());
	addJsonTypeConverter(new QJsonSizeConverter());
	addJsonTypeConverter(new QJsonPointConverter());
	addJsonTypeConverter(new QJsonLineConverter());
	addJsonTypeConverter(new QJsonRectConverter());
}

QJsonSerializer::~QJsonSerializer() {}

bool QJsonSerializer::allowDefaultNull() const
{
	return d->allowNull;
}

bool QJsonSerializer::keepObjectName() const
{
	return d->keepObjectName;
}

bool QJsonSerializer::enumAsString() const
{
	return d->enumAsString;
}

bool QJsonSerializer::validateBase64() const
{
	return d->validateBase64;
}

QJsonSerializer::ValidationFlags QJsonSerializer::validationFlags() const
{
	return d->validationFlags;
}

QJsonSerializer::Polymorphing QJsonSerializer::polymorphing() const
{
	return d->polymorphing;
}

QJsonValue QJsonSerializer::serialize(const QVariant &data) const
{
	return serializeImpl(data);
}

void QJsonSerializer::serializeTo(QIODevice *device, const QVariant &data) const
{
#ifndef QT_NO_DEBUG
	serializeToImpl(device, data, QJsonDocument::Indented);
#else
	serializeToImpl(device, data, QJsonDocument::Compact);
#endif
}

void QJsonSerializer::serializeTo(QIODevice *device, const QVariant &data, QJsonDocument::JsonFormat format) const
{
	serializeToImpl(device, data, format);
}

QByteArray QJsonSerializer::serializeTo(const QVariant &data) const
{
#ifndef QT_NO_DEBUG
	return serializeToImpl(data, QJsonDocument::Indented);
#else
	return serializeToImpl(data, QJsonDocument::Compact);
#endif
}

QByteArray QJsonSerializer::serializeTo(const QVariant &data, QJsonDocument::JsonFormat format) const
{
	return serializeToImpl(data, format);
}

QVariant QJsonSerializer::deserialize(const QJsonValue &json, int metaTypeId, QObject *parent) const
{
	return deserializeVariant(metaTypeId, json, parent);
}

QVariant QJsonSerializer::deserializeFrom(QIODevice *device, int metaTypeId, QObject *parent) const
{
	return deserializeVariant(metaTypeId, readFromDevice(device), parent);
}

QVariant QJsonSerializer::deserializeFrom(const QByteArray &data, int metaTypeId, QObject *parent) const
{
	QBuffer buffer(const_cast<QByteArray*>(&data));
	buffer.open(QIODevice::ReadOnly);
	auto res = deserializeFrom(&buffer, metaTypeId, parent);
	buffer.close();
	return res;
}

void QJsonSerializer::addJsonTypeConverter(QJsonTypeConverter *converter)
{
	Q_ASSERT_X(converter, Q_FUNC_INFO, "converter must not be null!");

	QSharedPointer<QJsonTypeConverter> sp(converter);
	foreach(auto jsonType, sp->jsonTypes()) {
		auto &convList = d->typeConverters[jsonType];
		auto inserted = false;

		for(auto i = 0; i < convList.size(); i++) {
			if(convList[i]->priority() <= sp->priority()) {
				convList.insert(i, sp);
				inserted = true;
				break;
			}
		}

		if(!inserted)
			convList.append(sp);
	}

	d->typeConverterTypeCache.clear();
}

void QJsonSerializer::setAllowDefaultNull(bool allowDefaultNull)
{
	d->allowNull = allowDefaultNull;
}

void QJsonSerializer::setKeepObjectName(bool keepObjectName)
{
	d->keepObjectName = keepObjectName;
}

void QJsonSerializer::setEnumAsString(bool enumAsString)
{
	d->enumAsString = enumAsString;
}

void QJsonSerializer::setValidateBase64(bool validateBase64)
{
	d->validateBase64 = validateBase64;
}

void QJsonSerializer::setValidationFlags(ValidationFlags validationFlags)
{
	d->validationFlags = validationFlags;
}

void QJsonSerializer::setPolymorphing(QJsonSerializer::Polymorphing polymorphing)
{
	d->polymorphing = polymorphing;
}

QVariant QJsonSerializer::getProperty(const char *name) const
{
	return property(name);
}

QJsonValue QJsonSerializer::serializeSubtype(QMetaProperty property, const QVariant &value) const
{
	QJsonExceptionContext ctx(property);
	if(property.isEnumType())
		return serializeEnum(property.enumerator(), value);
	else
		return serializeVariant(property.userType(), value);
}

QVariant QJsonSerializer::deserializeSubtype(QMetaProperty property, const QJsonValue &value, QObject *parent) const
{
	QJsonExceptionContext ctx(property);
	if(property.isEnumType())
		return deserializeEnum(property.enumerator(), value);
	else
		return deserializeVariant(property.userType(), value, parent);
}

QJsonValue QJsonSerializer::serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx(propertyType, traceHint);
	return serializeVariant(propertyType, value);
}

QVariant QJsonSerializer::deserializeSubtype(int propertyType, const QJsonValue &value, QObject *parent, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx(propertyType, traceHint);
	return deserializeVariant(propertyType, value, parent);
}

QJsonValue QJsonSerializer::serializeVariant(int propertyType, const QVariant &value) const
{
	auto converter = d->typeConverterTypeCache.value(propertyType, nullptr);
	if(!converter){
		foreach(auto cList, d->typeConverters) {
			foreach (auto c, cList) {
				if(c && c->canConvert(propertyType)) {
					converter = c;
					d->typeConverterTypeCache.insert(propertyType, converter);
					break;
				}
			}
		}
	}

	if(!converter)// use fallback method
		return serializeValue(propertyType, value);
	else
		return converter->serialize(propertyType, value, this);
}

QVariant QJsonSerializer::deserializeVariant(int propertyType, const QJsonValue &value, QObject *parent) const
{
	auto converter = d->typeConverterTypeCache.value(propertyType, nullptr);
	if(!converter || !converter->jsonTypes().contains(value.type())) {
		converter = nullptr; //in case json type did not match
		foreach (auto c, d->typeConverters.value(value.type())) {
			if(c && c->canConvert(propertyType)) {
				converter = c;
				d->typeConverterTypeCache.insert(propertyType, converter);
				break;
			}
		}
	}

	QVariant variant;
	if(!converter)// use fallback method
		variant = deserializeValue(propertyType, value);
	else
		variant = converter->deserialize(propertyType, value, parent, this);

	if(propertyType != QMetaType::UnknownType) {
		auto vType = variant.typeName();
		if(variant.canConvert(propertyType) && variant.convert(propertyType))
			return variant;
		else if(d->allowNull && value.isNull())
			return QVariant();
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

QJsonValue QJsonSerializer::serializeValue(int propertyType, const QVariant &value) const
{
	if(!value.isValid())
		return QJsonValue();
	else {
		if(value.userType() == QMetaType::QJsonValue)//value needs special treatment
			return value.value<QJsonValue>();

		auto json = QJsonValue::fromVariant(value);
		if(json.isNull()) { //special types where a null json is valid, and corresponds to a different
			static const QHash<int, QJsonValue::Type> nullTypes = {
				{QMetaType::Nullptr, QJsonValue::Null},
				{QMetaType::QDate, QJsonValue::String},
				{QMetaType::QTime, QJsonValue::String},
				{QMetaType::QDateTime, QJsonValue::String},
				{QMetaType::QUrl, QJsonValue::String}
			};

			auto typeMapping = nullTypes.value(propertyType, QJsonValue::Undefined);
			if(typeMapping == QJsonValue::Undefined)
				typeMapping = nullTypes.value(value.userType(), QJsonValue::Undefined);
			switch (typeMapping) {
			case QJsonValue::Null:
				return QJsonValue();
			case QJsonValue::Bool:
				return false;
			case QJsonValue::Double:
				return 0.0;
			case QJsonValue::String:
				return QString();
			case QJsonValue::Array:
				return QJsonArray();
			case QJsonValue::Object:
				return QJsonObject();
			case QJsonValue::Undefined:
				throw QJsonSerializationException(QByteArray("Failed to convert type ") +
												  value.typeName() +
												  QByteArray(" to a JSON representation"));
			default:
				Q_UNREACHABLE();
			}
		} else
			return json;
	}
}

QVariant QJsonSerializer::deserializeValue(int propertyType, const QJsonValue &value) const
{
	//all json can be converted to qvariant, but not all variant to type conversions do work
	switch (propertyType) {
	case QMetaType::QDate: //date, time, datetime -> empty string to d/t/dt fails, but is considered "correct" in this context
		if(value.toString().isEmpty())
			return QDate();
		break;
	case QMetaType::QTime:
		if(value.toString().isEmpty())
			return QTime();
		break;
	case QMetaType::QDateTime:
		if(value.toString().isEmpty())
			return QDateTime();
		break;
	default:
		break;
	}

	return value.toVariant();
}

QJsonValue QJsonSerializer::serializeEnum(const QMetaEnum &metaEnum, const QVariant &value) const
{
	if(d->enumAsString) {
		if(metaEnum.isFlag())
			return QString::fromUtf8(metaEnum.valueToKeys(value.toInt()));
		else
			return QString::fromUtf8(metaEnum.valueToKey(value.toInt()));
	} else
		return value.toInt();
}

QVariant QJsonSerializer::deserializeEnum(const QMetaEnum &metaEnum, const QJsonValue &value) const
{
	if(value.isString()) {
		auto result = -1;
		auto ok = false;
		if(metaEnum.isFlag())
			result = metaEnum.keysToValue(qUtf8Printable(value.toString()), &ok);
		else
			result = metaEnum.keyToValue(qUtf8Printable(value.toString()), &ok);
		if(ok)
			return result;
		else if(metaEnum.isFlag() && value.toString().isEmpty())
			return 0x00;
		else
			throw QJsonDeserializationException("Invalid value for enum type found: " + value.toString().toUtf8());
	} else {
		auto intValue = value.toInt();
		if(value.toDouble() != (double)intValue) {
			throw QJsonDeserializationException("Invalid value (double) for enum type found: " +
												QByteArray::number(value.toDouble()));
		}
		if(!metaEnum.isFlag() && metaEnum.valueToKey(intValue) == nullptr) {
			throw QJsonDeserializationException("Invalid integer value. Not a valid enum element: " +
												QByteArray::number(intValue));
		}
		return intValue;
	}
}

void QJsonSerializer::writeToDevice(const QJsonValue &data, QIODevice *device, QJsonDocument::JsonFormat format) const
{
	QJsonDocument doc;
	if(data.isArray())
		doc = QJsonDocument(data.toArray());
	else if(data.isObject())
		doc = QJsonDocument(data.toObject());
	else
		throw QJsonSerializationException("Only objects or arrays can be written to a device!");
	device->write(doc.toJson(format));
}

QJsonValue QJsonSerializer::readFromDevice(QIODevice *device) const
{
	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(device->readAll(), &error);
	if(error.error != QJsonParseError::NoError)
		throw QJsonDeserializationException("Failed to read file as JSON with error: " + error.errorString().toUtf8());
	if(doc.isArray())
		return doc.array();
	else
		return doc.object();
}

QJsonValue QJsonSerializer::serializeImpl(const QVariant &data) const
{
	return serializeVariant(data.userType(), data);
}

void QJsonSerializer::serializeToImpl(QIODevice *device, const QVariant &data) const
{
#ifndef QT_NO_DEBUG
	serializeToImpl(device, data, QJsonDocument::Indented);
#else
	serializeToImpl(device, data, QJsonDocument::Compact);
#endif
}

void QJsonSerializer::serializeToImpl(QIODevice *device, const QVariant &data, QJsonDocument::JsonFormat format) const
{
	writeToDevice(serializeVariant(data.userType(), data), device, format);
}

QByteArray QJsonSerializer::serializeToImpl(const QVariant &data) const
{
#ifndef QT_NO_DEBUG
	return serializeToImpl(data, QJsonDocument::Indented);
#else
	return serializeToImpl(data, QJsonDocument::Compact);
#endif
}

QByteArray QJsonSerializer::serializeToImpl(const QVariant &data, QJsonDocument::JsonFormat format) const
{
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	serializeToImpl(&buffer, data, format);
	buffer.close();
	return buffer.data();
}



QJsonSerializerPrivate::QJsonSerializerPrivate() :
	allowNull(false),
	keepObjectName(false),
	enumAsString(false),
	validateBase64(true),
	validationFlags(QJsonSerializer::StandardValidation),
	polymorphing(QJsonSerializer::Enabled)
{}

// ------------- Startup function implementation -------------

//use macro to register types to reduce number of symbols generated
#define REGISTER_DEFAULT_LIST(T) \
	ok = QMetaType::registerConverter<QList<T>, QVariantList>([](const QList<T> &list) -> QVariantList { \
		QVariantList l; \
		foreach(auto v, list) \
			l.append(QVariant::fromValue(v)); \
		return l; \
	}); \
	Q_ASSERT_X(ok, Q_FUNC_INFO, "Failed to register QList<T> -> QVariantList converter for type " #T); \
	\
	ok = QMetaType::registerConverter<QVariantList, QList<T>>([](const QVariantList &list) -> QList<T> { \
		QList<T> l; \
		foreach(auto v, list) { \
			auto vt = v.type(); \
			if(v.convert(qMetaTypeId<T>())) \
				l.append(v.value<T>()); \
			else { \
				qWarning() << "Conversion to" \
						   << QMetaType::typeName(qMetaTypeId<QList<T>>()) \
						   << "failed, could not convert element of type" \
						   << QMetaType::typeName(vt); \
				l.append(T()); \
			} \
		} \
		return l; \
	}); \
	Q_ASSERT_X(ok, Q_FUNC_INFO, "Failed to register QVariantList -> QList<T> converter for type " #T);

#define REGISTER_DEFAULT_MAP(T) \
	ok = QMetaType::registerConverter<QMap<QString, T>, QVariantMap>([](const QMap<QString, T> &map) -> QVariantMap { \
		QVariantMap m; \
		for(auto it = map.constBegin(); it != map.constEnd(); ++it) \
			m.insert(it.key(), QVariant::fromValue(it.value())); \
		return m; \
	}); \
	Q_ASSERT_X(ok, Q_FUNC_INFO, "Failed to register QMap<QString, T> -> QVariantMap converter for type " #T); \
	\
	ok = QMetaType::registerConverter<QVariantMap, QMap<QString, T>>([](const QVariantMap &map) -> QMap<QString, T> { \
		QMap<QString, T> m; \
		for(auto it = map.constBegin(); it != map.constEnd(); ++it) { \
			auto v = it.value(); \
			auto vt = v.type(); \
			if(v.convert(qMetaTypeId<T>())) \
				m.insert(it.key(), v.value<T>()); \
			else { \
				qWarning() << "Conversion to" \
						   << QMetaType::typeName(qMetaTypeId<QMap<QString, T>>()) \
						   << "failed, could not convert element value of type" \
						   << QMetaType::typeName(vt); \
				m.insert(it.key(), T()); \
			} \
		} \
		return m; \
	}); \
	Q_ASSERT_X(ok, Q_FUNC_INFO, "Failed to register QVariantMap -> QMap<QString, T> converter for type " #T);

#ifdef Q_JSONSERIALIZER_NO_MACRO
#define REGISTER_DEFAULT_CONVERTERS(T) QJsonSerializer::registerAllConverters<T>()
#else
#define REGISTER_DEFAULT_CONVERTERS(T) do { \
	bool ok = false; \
	REGISTER_DEFAULT_LIST(T) \
	REGISTER_DEFAULT_MAP(T) \
} while(false)
#endif

static void qJsonSerializerStartup()
{
	//primitive types
	REGISTER_DEFAULT_CONVERTERS(bool);
	REGISTER_DEFAULT_CONVERTERS(int);
	REGISTER_DEFAULT_CONVERTERS(uint);
	REGISTER_DEFAULT_CONVERTERS(qlonglong);
	REGISTER_DEFAULT_CONVERTERS(qulonglong);
	REGISTER_DEFAULT_CONVERTERS(double);
	REGISTER_DEFAULT_CONVERTERS(long);
	REGISTER_DEFAULT_CONVERTERS(short);
	REGISTER_DEFAULT_CONVERTERS(char);
	REGISTER_DEFAULT_CONVERTERS(ulong);
	REGISTER_DEFAULT_CONVERTERS(ushort);
	REGISTER_DEFAULT_CONVERTERS(uchar);
	REGISTER_DEFAULT_CONVERTERS(float);
	//macro does not work for the following two
	QJsonSerializer::registerAllConverters<signed char>();
	QJsonSerializer::registerAllConverters<QObject*>();

	//core types
	REGISTER_DEFAULT_CONVERTERS(QChar);
	REGISTER_DEFAULT_CONVERTERS(QString);
	REGISTER_DEFAULT_CONVERTERS(QDate);
	REGISTER_DEFAULT_CONVERTERS(QTime);
	REGISTER_DEFAULT_CONVERTERS(QDateTime);
	REGISTER_DEFAULT_CONVERTERS(QUrl);
	REGISTER_DEFAULT_CONVERTERS(QUuid);
	REGISTER_DEFAULT_CONVERTERS(QJsonValue);
	REGISTER_DEFAULT_CONVERTERS(QJsonObject);
	REGISTER_DEFAULT_CONVERTERS(QJsonArray);
	REGISTER_DEFAULT_CONVERTERS(QVersionNumber);
	REGISTER_DEFAULT_CONVERTERS(QSize);
	REGISTER_DEFAULT_CONVERTERS(QPoint);
	REGISTER_DEFAULT_CONVERTERS(QLine);
	REGISTER_DEFAULT_CONVERTERS(QRect);

	//extra: qbytearray
	{
		bool ok = false;
//		REGISTER_DEFAULT_LIST(QByteArray)
		REGISTER_DEFAULT_MAP(QByteArray)
	}
}
