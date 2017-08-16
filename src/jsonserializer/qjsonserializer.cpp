#include "qjsonserializer.h"
#include "qjsonserializer_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QUrl>
#include <QtCore/QJsonDocument>
#include <QtCore/QBuffer>

#include "typeconverters/qjsonobjectconverter_p.h"
#include "typeconverters/qjsongadgetconverter_p.h"
#include "typeconverters/qjsonmapconverter_p.h"
#include "typeconverters/qjsonlistconverter_p.h"
#include "typeconverters/qjsonjsonconverter_p.h"

static void qJsonSerializerStartup();
Q_COREAPP_STARTUP_FUNCTION(qJsonSerializerStartup)

QJsonSerializer::QJsonSerializer(QObject *parent) :
	QObject(parent),
	d(new QJsonSerializerPrivate())
{
	registerConverter(new QJsonObjectConverter());
	registerConverter(new QJsonGadgetConverter());
	registerConverter(new QJsonMapConverter());
	registerConverter(new QJsonListConverter());
	registerConverter(new QJsonJsonValueConverter());
	registerConverter(new QJsonJsonObjectConverter());
	registerConverter(new QJsonJsonArrayConverter());
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

QJsonSerializer::ValidationFlags QJsonSerializer::validationFlags() const
{
	return d->validationFlags;
}

QJsonValue QJsonSerializer::serialize(const QVariant &data) const
{
	return serializeImpl(data);
}

void QJsonSerializer::serializeTo(QIODevice *device, const QVariant &data) const
{
	serializeToImpl(device, data);
}

QByteArray QJsonSerializer::serializeTo(const QVariant &data) const
{
	return serializeToImpl(data);
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

void QJsonSerializer::registerConverter(QJsonTypeConverter *converter)
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

void QJsonSerializer::setValidationFlags(ValidationFlags validationFlags)
{
	d->validationFlags = validationFlags;
}

QVariant QJsonSerializer::getProperty(const char *name) const
{
	return property(name);
}

QJsonValue QJsonSerializer::serializeSubtype(QMetaProperty property, const QVariant &value) const
{
	if(property.isEnumType())
		return serializeEnum(property.enumerator(), value);
	else
		return serializeVariant(property.userType(), value);
}

QVariant QJsonSerializer::deserializeSubtype(QMetaProperty property, const QJsonValue &value, QObject *parent) const
{
	if(property.isEnumType())
		return deserializeEnum(property.enumerator(), value);
	else
		return deserializeVariant(property.userType(), value, parent);
}

QJsonValue QJsonSerializer::serializeSubtype(int propertyType, const QVariant &value) const
{
	return serializeVariant(propertyType, value);
}

QVariant QJsonSerializer::deserializeSubtype(int propertyType, const QJsonValue &value, QObject *parent) const
{
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
												QMetaType::typeName(propertyType));
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
		if(json.isNull()) {
			if(value.userType() == QMetaType::Nullptr)//std::nullptr_t is of course null
				return json;
			else if(propertyType == QMetaType::QDate ||
			   propertyType == QMetaType::QTime ||
			   propertyType == QMetaType::QDateTime ||
			   value.userType() == QMetaType::QDate ||
			   value.userType() == QMetaType::QTime ||
			   value.userType() == QMetaType::QDateTime)
				return QString();//special case date: invalid date -> empty string -> interpreted as fail -> thus return empty string
			else
				throw QJsonSerializationException(QByteArray("Failed to convert type ") +
												  value.typeName() +
												  QByteArray(" to a JSON representation"));
		}
		else
			return json;
	}
}

QVariant QJsonSerializer::deserializeValue(int propertyType, const QJsonValue &value) const
{
	Q_UNUSED(propertyType);
	return value.toVariant();//all json can be converted to qvariant
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

void QJsonSerializer::writeToDevice(const QJsonValue &data, QIODevice *device) const
{
	QJsonDocument doc;
	if(data.isArray())
		doc = QJsonDocument(data.toArray());
	else if(data.isObject())
		doc = QJsonDocument(data.toObject());
	else
		throw QJsonSerializationException("Only objects or arrays can be written to a device!");
#ifndef QT_NO_DEBUG
	device->write(doc.toJson(QJsonDocument::Indented));
#else
	device->write(doc.toJson(QJsonDocument::Compact));
#endif
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
	writeToDevice(serializeVariant(data.userType(), data), device);
}

QByteArray QJsonSerializer::serializeToImpl(const QVariant &data) const
{
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	serializeToImpl(&buffer, data);
	buffer.close();
	return buffer.data();
}



QJsonSerializerPrivate::QJsonSerializerPrivate() :
	allowNull(false),
	keepObjectName(false),
	enumAsString(false),
	validationFlags(QJsonSerializer::StandardValidation)
{}

// ------------- Startup function implementation -------------

static void qJsonSerializerStartup()
{
	QJsonSerializer::registerAllConverters<bool>();
	QJsonSerializer::registerAllConverters<int>();
	QJsonSerializer::registerAllConverters<unsigned int>();
	QJsonSerializer::registerAllConverters<double>();
	QJsonSerializer::registerAllConverters<QChar>();
	QJsonSerializer::registerAllConverters<QString>();
	QJsonSerializer::registerAllConverters<long long>();
	QJsonSerializer::registerAllConverters<short>();
	QJsonSerializer::registerAllConverters<char>();
	QJsonSerializer::registerAllConverters<unsigned long>();
	QJsonSerializer::registerAllConverters<unsigned long long>();
	QJsonSerializer::registerAllConverters<unsigned short>();
	QJsonSerializer::registerAllConverters<signed char>();
	QJsonSerializer::registerAllConverters<unsigned char>();
	QJsonSerializer::registerAllConverters<float>();
	QJsonSerializer::registerAllConverters<QDate>();
	QJsonSerializer::registerAllConverters<QTime>();
	QJsonSerializer::registerAllConverters<QUrl>();
	QJsonSerializer::registerAllConverters<QDateTime>();
	QJsonSerializer::registerAllConverters<QUuid>();
	QJsonSerializer::registerAllConverters<QObject*>();
}
