#include "qjsonobjectconverter_p.h"
#include "qjsonserializerexception.h"
#include "qjsonserializer_p.h"

#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>

bool QJsonObjectConverter::canConvert(int metaTypeId) const
{
	auto flags = QMetaType::typeFlags(metaTypeId);
	return flags.testFlag(QMetaType::PointerToQObject) ||
			flags.testFlag(QMetaType::SharedPointerToQObject) ||//weak ptr cannot be constructed
			flags.testFlag(QMetaType::TrackingPointerToQObject);//TODO test all types
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

	if(object) {
		auto meta = object->metaObject();
		auto d = QJsonSerializerPrivate::fromHelper(helper);

		QJsonObject jsonObject;
		//go through all properties and try to serialize them
		auto i = QObject::staticMetaObject.indexOfProperty("objectName");
		if(!d->keepObjectName)
		   i++;
		for(; i < meta->propertyCount(); i++) {
			auto property = meta->property(i);
			if(property.isStored())
				jsonObject[QString::fromUtf8(property.name())] = helper->serializeSubtype(property.userType(), property.read(object));
		}

		return jsonObject;
	}
	else
		return QJsonValue();
}

QVariant QJsonObjectConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
}

template<typename T>
T QJsonObjectConverter::extract(QVariant variant)
{
	auto id = qMetaTypeId<T>();
	if(variant.canConvert(id) && variant.convert(id))
		return variant.value<T>();
	else
		throw QJsonSerializationException(QByteArray("unable to get QObject pointer from type ") + QMetaType::typeName(id));
}
