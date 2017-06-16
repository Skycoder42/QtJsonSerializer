#include "qjsonserializer.h"
#include "qjsonserializer_p.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QUrl>
#include <QtCore/QJsonDocument>
#include <QtCore/QBuffer>

static const QRegularExpression listTypeRegex(QStringLiteral(R"__(^QList<\s*(.*)\s*>$)__"));
static const QRegularExpression mapTypeRegex(QStringLiteral(R"__(^QMap<\s*QString\s*,\s*(.*)\s*>$)__"));

static void qJsonSerializerStartup();
Q_COREAPP_STARTUP_FUNCTION(qJsonSerializerStartup)

QJsonSerializer::QJsonSerializer(QObject *parent) :
	QObject(parent),
	d(new QJsonSerializerPrivate())
{}

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

QJsonValue QJsonSerializer::serializeVariant(int propertyType, const QVariant &value) const
{
	auto convertValue = value;
	if((propertyType == QVariant::List) ||
	   (convertValue.canConvert(QVariant::List) && convertValue.convert(QVariant::List))) {
		return serializeList(propertyType, value.toList());
	} else if((propertyType == QVariant::Map) ||
			  (convertValue.canConvert(QVariant::Map) && convertValue.convert(QVariant::Map))) {
		return serializeMap(propertyType, value.toMap());
	} else{
		convertValue = value;
		auto flags = QMetaType::typeFlags(propertyType);

		if(flags.testFlag(QMetaType::IsGadget)) {
			auto metaObject = QMetaType::metaObjectForType(propertyType);
			return serializeGadget(value.constData(), metaObject);
		} else if(flags.testFlag(QMetaType::PointerToQObject) ||
				  (convertValue.canConvert(QMetaType::QObjectStar) && convertValue.convert(QMetaType::QObjectStar))) {
			auto object = convertValue.value<QObject*>();
			if(object)
				return serializeObject(object);
			else
				return QJsonValue::Null;
		} else
			return serializeValue(propertyType, value);
	}
}

QJsonObject QJsonSerializer::serializeObject(const QObject *object) const
{
	auto meta = object->metaObject();

	QJsonObject jsonObject;
	//go through all properties and try to serialize them
	auto i = QObject::staticMetaObject.indexOfProperty("objectName");
	if(!d->keepObjectName)
	   i++;
	for(; i < meta->propertyCount(); i++) {
		auto property = meta->property(i);
		if(property.isStored()) {
			QJsonValue value;
			if(property.isEnumType())
				value = serializeEnum(property.enumerator(), property.read(object));
			else
				value = serializeVariant(property.userType(), property.read(object));
			jsonObject[QString::fromUtf8(property.name())] = value;
		}
	}

	return jsonObject;
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

QJsonArray QJsonSerializer::serializeList(int listType, const QVariantList &value) const
{
	auto match = listTypeRegex.match(QString::fromUtf8(QMetaType::typeName(listType)));
	int metaType = QMetaType::UnknownType;
	if(match.hasMatch())
		metaType = QMetaType::type(match.captured(1).toLatin1().trimmed());

	QJsonArray array;
	foreach(auto element, value)
		array.append(serializeVariant(metaType, element));
	return array;
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
		return QJsonValue::Null;
	else {
		if(value.userType() == QMetaType::QJsonValue)//value needs special treatment
			return value.value<QJsonValue>();

		auto json = QJsonValue::fromVariant(value);
		if(json.isNull()) {
			if(propertyType == QMetaType::QDate ||
			   propertyType == QMetaType::QTime ||
			   propertyType == QMetaType::QDateTime ||
			   value.userType() == QMetaType::QDate ||
			   value.userType() == QMetaType::QTime ||
			   value.userType() == QMetaType::QDateTime)
				return QJsonValue::String;//special case date: invalid date -> empty string -> interpret as fail
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
	QVariant variant;
	if(propertyType == QMetaType::QJsonValue)//special case: target type is a json value!
		variant = QVariant::fromValue(value);
	else if(value.isArray()) {
		if(propertyType == QMetaType::QJsonArray)//special case: target type is a json array!
			variant = QVariant::fromValue(value.toArray());
		else
			variant = deserializeList(propertyType, value.toArray(), parent);
	} else if(value.isObject() || value.isNull()) {
		auto flags = QMetaType::typeFlags(propertyType);

		if(propertyType == QMetaType::QJsonObject)//special case: target type is a json object!
			variant = QVariant::fromValue(value.toObject());
		else if(propertyType == QMetaType::QVariantMap)
			variant = deserializeMap(propertyType, value.toObject(), parent);
		else if(flags.testFlag(QMetaType::IsGadget)) {
			if(!value.isNull()) {
				QVariant gadget(propertyType, nullptr);
				deserializeGadget(value.toObject(), propertyType, gadget.data());
				variant = gadget;
			}
		} else if(flags.testFlag(QMetaType::PointerToQObject)) {
			if(value.isNull())
				variant = QVariant::fromValue<QObject*>(nullptr);
			else {
				auto object = deserializeObject(value.toObject(), QMetaType::metaObjectForType(propertyType), parent);
				variant = QVariant::fromValue(object);
			}
		} else
			variant = deserializeValue(propertyType, value);
	} else
		variant = deserializeValue(propertyType, value);

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

QObject *QJsonSerializer::deserializeObject(const QJsonObject &jsonObject, const QMetaObject *metaObject, QObject *parent) const
{
	//try to construct the object
	auto object = metaObject->newInstance(Q_ARG(QObject*, parent));
	if(!object)
		throw QJsonDeserializationException(QByteArray("Failed to construct object of type ") +
											metaObject->className() +
											QByteArray(" (Does the constructor \"Q_INVOKABLE class(QObject*);\" exist?)"));

	//now deserialize all json properties
	for(auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++) {
		auto propIndex = metaObject->indexOfProperty(qUtf8Printable(it.key()));
		QVariant value;
		if(propIndex != -1) {
			auto property = metaObject->property(propIndex);
			if(property.isEnumType())
				value = deserializeEnum(property.enumerator(), it.value());
			else
				value = deserializeVariant(property.userType(), it.value(), object);
		} else if(d->validationFlags.testFlag(NoExtraProperties)) {
			throw QJsonDeserializationException(QByteArray("Found extra property ") +
												it.key().toUtf8() +
												" but extra properties are not allowed");
		} else
			deserializeVariant(QMetaType::UnknownType, it.value(), object);
		object->setProperty(qUtf8Printable(it.key()), value);
	}

	return object;
}

void QJsonSerializer::deserializeGadget(const QJsonObject &jsonObject, int typeId, void *gadgetPtr) const
{
	auto metaObject = QMetaType::metaObjectForType(typeId);
	if(!QMetaType::construct(typeId, gadgetPtr, nullptr))
		throw QJsonDeserializationException(QByteArray("Failed to construct gadget of type ") + QMetaType::typeName(typeId));

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
		} else if(d->validationFlags.testFlag(NoExtraProperties)) {
			throw QJsonDeserializationException(QByteArray("Found extra property ") +
												it.key().toUtf8() +
												" but extra properties are not allowed");
		}
	}
}

QVariantList QJsonSerializer::deserializeList(int listType, const QJsonArray &array, QObject *parent) const
{
	int metaType = QMetaType::UnknownType;
	if(listType != QMetaType::UnknownType) {
		auto match = listTypeRegex.match(QString::fromUtf8(QMetaType::typeName(listType)));
		if(match.hasMatch())
			metaType = QMetaType::type(match.captured(1).toLatin1().trimmed());
	}

	//generate the list
	QVariantList list;
	foreach(auto element, array)
		list.append(deserializeVariant(metaType, element, parent));
	return list;
}

QVariantMap QJsonSerializer::deserializeMap(int mapType, const QJsonObject &object, QObject *parent) const
{
	int metaType = QMetaType::UnknownType;
	if(mapType != QMetaType::UnknownType) {
		auto match = mapTypeRegex.match(QString::fromUtf8(QMetaType::typeName(mapType)));
		if(match.hasMatch())
			metaType = QMetaType::type(match.captured(1).toLatin1().trimmed());
	}

	//generate the list
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
			result = metaEnum.keysToValue(value.toString().toUtf8().constData(), &ok);
		else
			result = metaEnum.keyToValue(value.toString().toUtf8().constData(), &ok);
		if(ok)
			return result;
		else if(metaEnum.isFlag() && value.toString().isEmpty())
			return QVariant();
		else
			throw QJsonDeserializationException("Invalid value for enum type found");
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
