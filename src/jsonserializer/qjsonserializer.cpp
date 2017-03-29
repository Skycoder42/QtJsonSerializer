#include "qjsonserializer.h"
#include "qjsonserializer_p.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QUrl>

static const QRegularExpression listTypeRegex(QStringLiteral(R"__(^QList<\s*(.*)\s*>$)__"));

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

void QJsonSerializer::setAllowDefaultNull(bool allowDefaultNull)
{
	d->allowNull = allowDefaultNull;
}

void QJsonSerializer::setKeepObjectName(bool keepObjectName)
{
	d->keepObjectName = keepObjectName;
}

QJsonValue QJsonSerializer::serializeVariant(int propertyType, const QVariant &value) const
{
	auto convertValue = value;
	if((propertyType == QVariant::List) ||
	   (convertValue.canConvert(QVariant::List) && convertValue.convert(QVariant::List))) {
		return serializeList(propertyType, value.toList());
	} else {
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
		if(property.isStored())
			jsonObject[QString::fromUtf8(property.name())]= serializeVariant(property.userType(), property.read(object));
	}

	return jsonObject;
}

QJsonObject QJsonSerializer::serializeGadget(const void *gadget, const QMetaObject *metaObject) const
{
	QJsonObject jsonObject;
	//go through all properties and try to serialize them
	for(auto i = 0; i < metaObject->propertyCount(); i++) {
		auto property = metaObject->property(i);
		if(property.isStored())
			jsonObject[QString::fromUtf8(property.name())]= serializeVariant(property.userType(), property.readOnGadget(gadget));
	}

	return jsonObject;
}

QJsonArray QJsonSerializer::serializeList(int listType, const QVariantList &value) const
{
	auto match = listTypeRegex.match(QString::fromUtf8(QMetaType::typeName(listType)));
	int metaType = QMetaType::UnknownType;
	if(match.hasMatch())
		metaType = QMetaType::type(match.captured(1).toLatin1());

	QJsonArray array;
	foreach(auto element, value)
		array.append(serializeVariant(metaType, element));
	return array;
}

QJsonValue QJsonSerializer::serializeValue(int propertyType, const QVariant &value) const
{
	if(!value.isValid())
		return QJsonValue::Null;
	else {
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
	if(value.isArray()) {
		variant = deserializeList(propertyType, value.toArray(), parent);
	} else if(value.isObject() || value.isNull()) {
		auto flags = QMetaType::typeFlags(propertyType);

		if(flags.testFlag(QMetaType::IsGadget)) {
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
		auto type = propIndex != -1 ?
						metaObject->property(propIndex).userType() :
						QMetaType::UnknownType;
		object->setProperty(qUtf8Printable(it.key()), deserializeVariant(type, it.value(), object));
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
			property.writeOnGadget(gadgetPtr, deserializeVariant(property.userType(), it.value(), nullptr));
		}
	}
}

QVariantList QJsonSerializer::deserializeList(int listType, const QJsonArray &array, QObject *parent) const
{
	int metaType = QMetaType::UnknownType;
	if(listType != QMetaType::UnknownType) {
		auto match = listTypeRegex.match(QString::fromUtf8(QMetaType::typeName(listType)));
		if(match.hasMatch())
			metaType = QMetaType::type(match.captured(1).toLatin1());
	}

	//generate the list
	QVariantList list;
	foreach(auto element, array)
		list.append(deserializeVariant(metaType, element, parent));
	return list;
}

QVariant QJsonSerializer::deserializeValue(int propertyType, const QJsonValue &value) const
{
	Q_UNUSED(propertyType);
	return value.toVariant();//all json can be converted to qvariant
}



QJsonSerializerPrivate::QJsonSerializerPrivate() :
	allowNull(false),
	keepObjectName(false)
{}

// ------------- Startup function implementation -------------

static void qJsonSerializerStartup()
{
	QJsonSerializer::registerListConverters<bool>();
	QJsonSerializer::registerListConverters<int>();
	QJsonSerializer::registerListConverters<unsigned int>();
	QJsonSerializer::registerListConverters<double>();
	QJsonSerializer::registerListConverters<QChar>();
	QJsonSerializer::registerListConverters<QString>();
	QJsonSerializer::registerListConverters<long long>();
	QJsonSerializer::registerListConverters<short>();
	QJsonSerializer::registerListConverters<char>();
	QJsonSerializer::registerListConverters<unsigned long>();
	QJsonSerializer::registerListConverters<unsigned long long>();
	QJsonSerializer::registerListConverters<unsigned short>();
	QJsonSerializer::registerListConverters<signed char>();
	QJsonSerializer::registerListConverters<unsigned char>();
	QJsonSerializer::registerListConverters<float>();
	QJsonSerializer::registerListConverters<QDate>();
	QJsonSerializer::registerListConverters<QTime>();
	QJsonSerializer::registerListConverters<QUrl>();
	QJsonSerializer::registerListConverters<QDateTime>();
	QJsonSerializer::registerListConverters<QUuid>();
	QJsonSerializer::registerListConverters<QObject*>();
}
