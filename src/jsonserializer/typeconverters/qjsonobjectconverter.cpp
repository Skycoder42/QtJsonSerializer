#include "qjsonobjectconverter_p.h"
#include "qjsonserializerexception.h"
#include "qjsonserializer_p.h"

#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>

const QRegularExpression QJsonObjectConverter::sharedTypeRegex(QStringLiteral(R"__(^QSharedPointer<\s*(.*?)\s*>$)__"));
const QRegularExpression QJsonObjectConverter::trackingTypeRegex(QStringLiteral(R"__(^QPointer<\s*(.*?)\s*>$)__"));

bool QJsonObjectConverter::canConvert(int metaTypeId) const
{
	auto flags = QMetaType::typeFlags(metaTypeId);
	return flags.testFlag(QMetaType::PointerToQObject) ||
			flags.testFlag(QMetaType::SharedPointerToQObject) ||//weak ptr cannot be constructed
			flags.testFlag(QMetaType::TrackingPointerToQObject);
}

QList<QJsonValue::Type> QJsonObjectConverter::jsonTypes() const
{
	return {
		QJsonValue::Object,
		QJsonValue::Null
	};
}

QJsonValue QJsonObjectConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	QObject *object = nullptr;
	auto flags = QMetaType::typeFlags(propertyType);
	if(flags.testFlag(QMetaType::PointerToQObject))
	  object = extract<QObject*>(value);
	else if(flags.testFlag(QMetaType::SharedPointerToQObject))
	  object = extract<QSharedPointer<QObject>>(value).data();
	else if(flags.testFlag(QMetaType::TrackingPointerToQObject))
	  object = extract<QPointer<QObject>>(value).data();
	else
	  Q_UNREACHABLE();

	if(!object)
		return QJsonValue();

	//get the metaobject, based on polymorphism
	const QMetaObject *meta = nullptr;
	auto poly = static_cast<QJsonSerializer::Polymorphing>(helper->getProperty("polymorphing").toInt());
	auto isPoly = false;
	switch (poly) {
	case QJsonSerializer::Disabled:
		isPoly = false;
		break;
	case QJsonSerializer::Enabled:
		isPoly = polyMetaObject(object);
		break;
	case QJsonSerializer::Forced:
		isPoly = true;
		break;
	default:
		Q_UNREACHABLE();
		break;
	}

	QJsonObject jsonObject;

	if(isPoly) {
		meta = object->metaObject();
		//first: pass the class name
		jsonObject[QStringLiteral("@class")] = QString::fromUtf8(meta->className());
	} else
		meta = getMetaObject(propertyType);

	//go through all properties and try to serialize them
	auto keepObjectName = helper->getProperty("keepObjectName").toBool();
	auto i = QObject::staticMetaObject.indexOfProperty("objectName");
	if(!keepObjectName)
	   i++;
	for(; i < meta->propertyCount(); i++) {
		auto property = meta->property(i);
		if(property.isStored())
			jsonObject[QString::fromUtf8(property.name())] = helper->serializeSubtype(property, property.read(object));
	}

	return jsonObject;
}

QVariant QJsonObjectConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	if(value.isNull())
		return toVariant(nullptr, QMetaType::typeFlags(propertyType));

	auto validationFlags = helper->getProperty("validationFlags").value<QJsonSerializer::ValidationFlags>();
	auto keepObjectName = helper->getProperty("keepObjectName").toBool();
	auto poly = static_cast<QJsonSerializer::Polymorphing>(helper->getProperty("polymorphing").toInt());

	auto metaObject = getMetaObject(propertyType);
	if(!metaObject)
		throw QJsonDeserializationException(QByteArray("Unable to get metaobject for type ") + QMetaType::typeName(propertyType));

	//try to get the polymorphic metatype (if allowed)
	auto jsonObject = value.toObject();
	if(poly != QJsonSerializer::Disabled) {
		if(jsonObject.contains(QStringLiteral("@class"))) {
			QByteArray classField = jsonObject[QStringLiteral("@class")].toString().toUtf8() + "*";//add the star
			auto typeId = QMetaType::type(classField.constData());
			auto nMeta = QMetaType::metaObjectForType(typeId);
			if(!nMeta)
				throw QJsonDeserializationException("Unable to find class requested from json \"@class\" property: " + classField);
			if(!nMeta->inherits(metaObject)) {
				throw QJsonDeserializationException("Requested class from \"@class\" field, " +
													classField +
													QByteArray(", does not inhert the property type ") +
													QMetaType::typeName(propertyType));
			}
			metaObject = nMeta;
		} else if(poly == QJsonSerializer::Forced)
			throw QJsonDeserializationException("Json does not contain the \"@class\" field, but forced polymorphism requires it");
	}

	//try to construct the object
	auto object = metaObject->newInstance(Q_ARG(QObject*, parent));
	if(!object) {
		throw QJsonDeserializationException(QByteArray("Failed to construct object of type ") +
											metaObject->className() +
											QByteArray(" (Does the constructor \"Q_INVOKABLE class(QObject*);\" exist?)"));
	}

	//collect required properties, if set
	QSet<QByteArray> reqProps;
	if(validationFlags.testFlag(QJsonSerializer::AllProperties)) {
		auto i = QObject::staticMetaObject.indexOfProperty("objectName");
		if(!keepObjectName)
		   i++;
		for(; i < metaObject->propertyCount(); i++) {
			auto property = metaObject->property(i);
			if(property.isStored())
				reqProps.insert(property.name());
		}
	}

	//now deserialize all json properties
	for(auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++) {
		auto propIndex = metaObject->indexOfProperty(qUtf8Printable(it.key()));
		QVariant subValue;
		if(propIndex != -1) {
			auto property = metaObject->property(propIndex);
			subValue = helper->deserializeSubtype(property, it.value(), object);
			reqProps.remove(property.name());
		} else if(validationFlags.testFlag(QJsonSerializer::NoExtraProperties)) {
			throw QJsonDeserializationException("Found extra property " +
												it.key().toUtf8() +
												" but extra properties are not allowed");
		} else
			subValue = helper->deserializeSubtype(QMetaType::UnknownType, it.value(), object, it.key().toUtf8());
		object->setProperty(qUtf8Printable(it.key()), subValue);
	}

	//make shure all required properties have been read
	if(validationFlags.testFlag(QJsonSerializer::AllProperties) && !reqProps.isEmpty()) {
		throw QJsonDeserializationException(QByteArray("Not all properties for ") +
											metaObject->className() +
											QByteArray(" are present in the json object Missing properties: ") +
											reqProps.toList().join(", "));
	}

	return toVariant(object, QMetaType::typeFlags(propertyType));
}

const QMetaObject *QJsonObjectConverter::getMetaObject(int typeId)
{
	auto flags = QMetaType::typeFlags(typeId);
	if(flags.testFlag(QMetaType::PointerToQObject))
		return QMetaType::metaObjectForType(typeId);
	else {
		QRegularExpression regex;
		if(flags.testFlag(QMetaType::SharedPointerToQObject))
			regex = sharedTypeRegex;
		else if(flags.testFlag(QMetaType::TrackingPointerToQObject))
			regex = trackingTypeRegex;
		else {
			Q_UNREACHABLE();
			return nullptr;
		}

		//extract template type, and if found, add the pointer star and find the meta type
		auto match = regex.match(QString::fromUtf8(QMetaType::typeName(typeId)));
		if(match.hasMatch()) {
			auto type = QMetaType::type(match.captured(1).toUtf8().trimmed() + "*");
			return QMetaType::metaObjectForType(type);
		} else
			return nullptr;
	}
}

template<typename T>
T QJsonObjectConverter::extract(QVariant variant)
{
	auto id = qMetaTypeId<T>();
	if(variant.canConvert(id) && variant.convert(id))
		return variant.value<T>();
	else {
		throw QJsonSerializationException(QByteArray("unable to get QObject pointer from type ") +
										  QMetaType::typeName(id) +
										  QByteArray(". Make shure to register pointer converters via QJsonSerializer::registerPointerConverters"));
	}
}

QVariant QJsonObjectConverter::toVariant(QObject *object, QMetaType::TypeFlags flags)
{
	if(flags.testFlag(QMetaType::PointerToQObject))
		return QVariant::fromValue(object);
	else if(flags.testFlag(QMetaType::SharedPointerToQObject)) {
		//remove parent, as shared pointers and object tree exclude each other
		if(object)
			object->setParent(nullptr);
		return QVariant::fromValue(QSharedPointer<QObject>(object));
	} else if(flags.testFlag(QMetaType::TrackingPointerToQObject))
		return QVariant::fromValue<QPointer<QObject>>(object);
	else {
		Q_UNREACHABLE();
		return QVariant();
	}
}

bool QJsonObjectConverter::polyMetaObject(QObject *object)
{
	auto meta = object->metaObject();

	//check the internal property
	if(object->dynamicPropertyNames().contains("__qt_json_serializer_polymorphic")) {
		auto polyProp = object->property("__qt_json_serializer_polymorphic").toBool();
		return polyProp;
	}

	//check the class info
	auto polyIndex = meta->indexOfClassInfo("polymorphic");
	if(polyIndex != -1) {
		auto info = meta->classInfo(polyIndex);
		if(info.value() == QByteArray("true"))
			return true;// use the object
		else if(info.value() == QByteArray("false"))
			return false;// use the class
		else
			qWarning() << "Invalid value for polymorphic classinfo on object type" << meta->className() << "ignored";
	}

	//default: the class
	return false;// use the class
}
