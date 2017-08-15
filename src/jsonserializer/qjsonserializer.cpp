#include "qjsonserializer.h"
#include "qjsonserializer_p.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QUrl>
#include <QtCore/QJsonDocument>
#include <QtCore/QBuffer>

#include "typeconverters/qjsonlistconverter_p.h"
#include "typeconverters/qjsonobjectconverter_p.h"

static const QRegularExpression listTypeRegex(QStringLiteral(R"__(^QList<\s*(.*)\s*>$)__"));
static const QRegularExpression mapTypeRegex(QStringLiteral(R"__(^QMap<\s*QString\s*,\s*(.*)\s*>$)__"));

static void qJsonSerializerStartup();
Q_COREAPP_STARTUP_FUNCTION(qJsonSerializerStartup)

QJsonSerializer::QJsonSerializer(QObject *parent) :
	QObject(parent),
	d(new QJsonSerializerPrivate())
{
	registerConverter(new QJsonListConverter());
	registerConverter(new QJsonObjectConverter());
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
	QSharedPointer<QJsonTypeConverter> sp(converter);
	foreach(auto jsonType, converter->jsonTypes())
		d->typeConverters.insert(jsonType, sp);
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
		foreach(auto c, d->typeConverters) {
			if(c && c->canConvert(propertyType)) {
				converter = c;
				d->typeConverterTypeCache.insert(propertyType, converter);
				break;
			}
		}
	}

	if(!converter)// use fallback method
		return serializeValue(propertyType, value);
	else
		return converter->serialize(propertyType, value, this);

//	//old implementation
//	auto convertValue = value;
//	if((propertyType == QVariant::Map) ||
//			  (convertValue.canConvert(QVariant::Map) && convertValue.convert(QVariant::Map))) {
//		return serializeMap(propertyType, value.toMap());
//	} else {
//		convertValue = value;
//		auto flags = QMetaType::typeFlags(propertyType);

//		if(flags.testFlag(QMetaType::IsGadget)) {
//			auto metaObject = QMetaType::metaObjectForType(propertyType);
//			return serializeGadget(value.constData(), metaObject);
//		} else
//			return serializeValue(propertyType, value);
//	}
}

QJsonObject QJsonSerializer::serializeGadget(const void *gadget, const QMetaObject *metaObject) const
{
	QJsonObject jsonObject;
	//go through all properties and try to serialize them
	for(auto i = 0; i < metaObject->propertyCount(); i++) {
		auto property = metaObject->property(i);
		if(property.isStored()) {
			QJsonValue value;
			if(property.isEnumType())
				value = serializeEnum(property.enumerator(), property.readOnGadget(gadget));
			else
				value = serializeVariant(property.userType(), property.readOnGadget(gadget));
			jsonObject[QString::fromUtf8(property.name())] = value;
		}
	}

	return jsonObject;
}

QJsonObject QJsonSerializer::serializeMap(int mapType, const QVariantMap &value) const
{
	auto match = mapTypeRegex.match(QString::fromUtf8(QMetaType::typeName(mapType)));
	int metaType = QMetaType::UnknownType;
	if(match.hasMatch())
		metaType = QMetaType::type(match.captured(1).toLatin1().trimmed());

	QJsonObject object;
	for(auto it = value.constBegin(); it != value.constEnd(); ++it)
		object.insert(it.key(), serializeVariant(metaType, it.value()));
	return object;
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

QVariant QJsonSerializer::deserializeVariant(int propertyType, const QJsonValue &value, QObject *parent) const
{
	auto converter = d->typeConverterTypeCache.value(propertyType, nullptr);
	if(!converter || !converter->jsonTypes().contains(value.type())){
		foreach(auto c, d->typeConverters.values(value.type())) {
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

//	//old implementation
//	QVariant variant;
//	if(propertyType == QMetaType::QJsonValue)//special case: target type is a json value!
//		variant = QVariant::fromValue(value);
//	else if(value.isArray()) {
//		if(propertyType == QMetaType::QJsonArray)//special case: target type is a json array!
//			variant = QVariant::fromValue(value.toArray());
//	} else if(value.isObject() || value.isNull()) {
//		auto flags = QMetaType::typeFlags(propertyType);

//		if(propertyType == QMetaType::QJsonObject)//special case: target type is a json object!
//			variant = QVariant::fromValue(value.toObject());
//		else if(propertyType == QMetaType::QVariantMap ||
//				mapTypeRegex.match(QString::fromUtf8(QMetaType::typeName(propertyType))).hasMatch())
//			variant = deserializeMap(propertyType, value.toObject(), parent);
//		else if(flags.testFlag(QMetaType::IsGadget)) {
//			if(!value.isNull()) {
//				QVariant gadget(propertyType, nullptr);
//				deserializeGadget(value.toObject(), propertyType, gadget.data());
//				variant = gadget;
//			}
//		} else
//			variant = deserializeValue(propertyType, value);
//	} else
//		variant = deserializeValue(propertyType, value);

//	if(propertyType != QMetaType::UnknownType) {
//		auto vType = variant.typeName();
//		if(variant.canConvert(propertyType) && variant.convert(propertyType))
//			return variant;
//		else if(d->allowNull && value.isNull())
//			return QVariant();
//		else {
//			throw QJsonDeserializationException(QByteArray("Failed to convert deserialized variant of type ") +
//												(vType ? vType : "<unknown>") +
//												QByteArray(" to property type ") +
//												QMetaType::typeName(propertyType));
//		}
//	} else
//		return variant;
}

void QJsonSerializer::deserializeGadget(const QJsonObject &jsonObject, int typeId, void *gadgetPtr) const
{
	auto metaObject = QMetaType::metaObjectForType(typeId);
	if(!QMetaType::construct(typeId, gadgetPtr, nullptr))
		throw QJsonDeserializationException(QByteArray("Failed to construct gadget of type ") + QMetaType::typeName(typeId));

	QSet<QByteArray> reqProps;
	if(d->validationFlags.testFlag(AllProperties)) {
		for(auto i = 0; i < metaObject->propertyCount(); i++) {
			auto property = metaObject->property(i);
			if(property.isStored())
				reqProps.insert(property.name());
		}
	}

	//now deserialize all json properties
	for(auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++) {
		auto propIndex = metaObject->indexOfProperty(qUtf8Printable(it.key()));
		if(propIndex != -1) {
			auto property = metaObject->property(propIndex);
			QVariant value;
			if(property.isEnumType())
				value = deserializeEnum(property.enumerator(), it.value());
			else
				value = deserializeVariant(property.userType(), it.value(), nullptr);
			property.writeOnGadget(gadgetPtr, value);
			reqProps.remove(property.name());
		} else if(d->validationFlags.testFlag(NoExtraProperties)) {
			throw QJsonDeserializationException("Found extra property " +
												it.key().toUtf8() +
												" but extra properties are not allowed");
		}
	}

	if(d->validationFlags.testFlag(AllProperties) && !reqProps.isEmpty()) {
		throw QJsonDeserializationException(QByteArray("Not all properties for ") +
											metaObject->className() +
											QByteArray(" are present in the json object. Missing properties: ") +
											reqProps.toList().join(", "));
	}
}

QVariantMap QJsonSerializer::deserializeMap(int mapType, const QJsonObject &object, QObject *parent) const
{
	int metaType = QMetaType::UnknownType;
	if(mapType != QMetaType::UnknownType) {
		auto match = mapTypeRegex.match(QString::fromUtf8(QMetaType::typeName(mapType)));
		if(match.hasMatch())
			metaType = QMetaType::type(match.captured(1).toLatin1().trimmed());
	}

	//generate the map
	QVariantMap map;
	for(auto it = object.constBegin(); it != object.constEnd(); ++it)
		map.insert(it.key(), deserializeVariant(metaType, it.value(), parent));
	return map;
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
	} else
		return value.toInt();
}

QVariant QJsonSerializer::deserializeValue(int propertyType, const QJsonValue &value) const
{
	Q_UNUSED(propertyType);
	return value.toVariant();//all json can be converted to qvariant
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

QJsonSerializerPrivate *QJsonSerializerPrivate::fromHelper(const QJsonTypeConverter::SerializationHelper *helper)
{
	auto jser = dynamic_cast<const QJsonSerializer*>(helper);
	if(jser)
		return jser->d.data();
	else
		return nullptr;
}

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
