#include "qjsongadgetconverter_p.h"
#include "qjsonserializerexception.h"
#include "qjsonserializer_p.h"

#include <QtCore/QMetaProperty>

bool QJsonGadgetConverter::canConvert(int metaTypeId) const
{
	return QMetaType::typeFlags(metaTypeId).testFlag(QMetaType::IsGadget);
}

QList<QJsonValue::Type> QJsonGadgetConverter::jsonTypes() const
{
	return {QJsonValue::Object};
}

QJsonValue QJsonGadgetConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto metaObject = QMetaType::metaObjectForType(propertyType);
	if(!metaObject)
		throw QJsonSerializationException(QByteArray("Unable to get metaobject for type") + QMetaType::typeName(propertyType));

	auto gadget = value.constData();
	QJsonObject jsonObject;
	//go through all properties and try to serialize them
	for(auto i = 0; i < metaObject->propertyCount(); i++) {
		auto property = metaObject->property(i);
		if(property.isStored())
			jsonObject[QString::fromUtf8(property.name())] = helper->serializeSubtype(property, property.readOnGadget(gadget));
	}

	return jsonObject;
}

QVariant QJsonGadgetConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(parent)//gadgets neither have nor serve as parent

	QVariant gadget(propertyType, nullptr);
	auto gadgetPtr = gadget.data();
	if(!gadgetPtr)
		throw QJsonDeserializationException(QByteArray("Failed to construct gadget of type ") + QMetaType::typeName(propertyType));

	auto metaObject = QMetaType::metaObjectForType(propertyType);
	if(!metaObject)
		throw QJsonDeserializationException(QByteArray("Unable to get metaobject for type") + QMetaType::typeName(propertyType));

	auto jsonObject = value.toObject();
	auto validationFlags = helper->getProperty("validationFlags").value<QJsonSerializer::ValidationFlags>();

	//collect required properties, if set
	QSet<QByteArray> reqProps;
	if(validationFlags.testFlag(QJsonSerializer::AllProperties)) {
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
			auto value = helper->deserializeSubtype(property, it.value(), nullptr);
			property.writeOnGadget(gadgetPtr, value);
			reqProps.remove(property.name());
		} else if(validationFlags.testFlag(QJsonSerializer::NoExtraProperties)) {
			throw QJsonDeserializationException("Found extra property " +
												it.key().toUtf8() +
												" but extra properties are not allowed");
		}
	}

	//make shure all required properties have been read
	if(validationFlags.testFlag(QJsonSerializer::AllProperties) && !reqProps.isEmpty()) {
		throw QJsonDeserializationException(QByteArray("Not all properties for ") +
											metaObject->className() +
											QByteArray(" are present in the json object. Missing properties: ") +
											reqProps.toList().join(", "));
	}

	return gadget;
}
