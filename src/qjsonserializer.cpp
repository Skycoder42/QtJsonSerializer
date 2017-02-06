#include "qjsonserializer.h"

#include <QDebug>
#include <QJsonArray>

class QJsonSerializerPrivate
{
public:
	QJsonSerializerPrivate(QJsonSerializer *q_ptr);

	bool allowNull;

	QJsonValue doSerialize(const QMetaProperty &property, const QVariant &value);
	QJsonObject serializeObject(const QObject *object);

	QVariant doDeserialize(const QMetaProperty &property, const QJsonValue &value, QObject *parent, int overrideTypeId = -1);
	QObject *deserializeObject(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent);

private:
	QJsonSerializer *q_ptr;
};

#define d d_ptr

QJsonSerializer::QJsonSerializer(QObject *parent) :
	QObject(parent),
	d_ptr(new QJsonSerializerPrivate(this))
{}

QJsonSerializer::~QJsonSerializer() {}

bool QJsonSerializer::allowDefaultNull() const
{
	return d->allowNull;
}

QJsonObject QJsonSerializer::serialize(const QObject *object) const
{
	return d->serializeObject(object);
}

QObject *QJsonSerializer::deserialize(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent) const
{
	return d->deserializeObject(jsonObject, metaObject, parent);
}

QList<QObject *> QJsonSerializer::deserialize(QJsonArray jsonArray, const QMetaObject *metaObject, QObject *parent) const
{
	QList<QObject*> list;
	foreach(auto json, jsonArray) {
		if(json.isObject())
			list.append(deserialize(json.toObject(), metaObject, parent));
		else {
			throw SerializerException(QStringLiteral("Failed convert array element of type %1 to %2")
									  .arg(json.type())
									  .arg(metaObject->className()),
									  true);
		}
	}
	return list;
}

void QJsonSerializer::setAllowDefaultNull(bool allowDefaultNull)
{
	d->allowNull = allowDefaultNull;
}

QJsonValue QJsonSerializer::serializeValue(QVariant value)
{
	if(!value.isValid())
		return QJsonValue::Null;
	else {
		auto json = QJsonValue::fromVariant(value);
		if(json.isNull())
			throw SerializerException(QStringLiteral("Failed to convert type %1 to a JSON representation").arg(value.typeName()), false);
		else
			return json;
	}
}

QVariant QJsonSerializer::deserializeValue(QJsonValue value)
{
	return value.toVariant();//all json can be converted to qvariant
}

// ------------- Private Implementation -------------

QJsonSerializerPrivate::QJsonSerializerPrivate(QJsonSerializer *q_ptr) :
	allowNull(false),
	q_ptr(q_ptr)
{}

QJsonValue QJsonSerializerPrivate::doSerialize(const QMetaProperty &property, const QVariant &value)
{
	auto lValue = value;
	if((property.isValid() && property.type() == QVariant::List) ||
	   (lValue.canConvert(QVariant::List) && lValue.convert(QVariant::List))) {
		QJsonArray array;
		foreach(auto element, lValue.toList())
			array.append(doSerialize({}, element));
		return array;
	} else {
		lValue = value;
		auto restId = qMetaTypeId<QObject*>();
		if(lValue.canConvert(restId) && lValue.convert(restId)) {
			auto object = lValue.value<QObject*>();
			if(object)
				return serializeObject(object);
			else
				return QJsonValue::Null;
		} else
			return q_ptr->serializeValue(value);
	}
}

QJsonObject QJsonSerializerPrivate::serializeObject(const QObject *object)
{
	auto meta = object->metaObject();

	QJsonObject jsonObject;
	//go through all properties and try to serialize them
	for(auto i = QObject::staticMetaObject.propertyOffset(); i < meta->propertyCount(); i++) {
		auto property = meta->property(i);
		if(property.isStored())
			jsonObject[property.name()]= doSerialize(property, property.read(object));
	}

	return jsonObject;
}

QVariant QJsonSerializerPrivate::doDeserialize(const QMetaProperty &property, const QJsonValue &value, QObject *parent, int overrideTypeId)
{
	QVariant variant;
	if(value.isArray()) {
		//check for a registered list object type
		auto oTypeId = -1;
		if(property.isValid()) {
			auto ok = false;//TODO parse type name instead
			oTypeId = parent->property((QByteArray("__qtrc_ro_olp_") + property.name()).constData()).toInt(&ok);
			if(!ok)
				oTypeId = -1;
		}

		//generate the list
		QVariantList vList;
		foreach(auto element, value.toArray())
			vList.append(doDeserialize({}, element, parent, oTypeId));
		variant = vList;
	} else if((property.isValid() || overrideTypeId != -1) &&
			  (value.isObject() || value.isNull())) {
		auto rId = overrideTypeId != -1 ? overrideTypeId : property.userType();
		QMetaType mType(rId);
		if(mType.flags().testFlag(QMetaType::PointerToQObject) &&
		   mType.metaObject()->inherits(&QObject::staticMetaObject)) {
			if(value.isNull())
				variant = QVariant::fromValue<QObject*>(nullptr);
			else {
				auto restObj = deserializeObject(value.toObject(), mType.metaObject(), parent);
				variant = QVariant::fromValue(restObj);
			}
		} else
			variant = q_ptr->deserializeValue(value);
	} else
		variant = q_ptr->deserializeValue(value);

	if(property.isValid()) {
		auto vType = variant.typeName();
		if(variant.canConvert(property.userType()) && variant.convert(property.userType()))
			return variant;
		else if(allowNull && value.isNull())
			return QVariant();
		else {
			throw SerializerException(QStringLiteral("Failed to convert deserialized variant of type %1 to property type %2")
									  .arg(vType ? vType : "<unknown>")
									  .arg(property.typeName()),
									  true);
		}
	} else
		return variant;
}

QObject *QJsonSerializerPrivate::deserializeObject(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent)
{
	//try to construct the object
	auto object = qobject_cast<QObject*>(metaObject->newInstance(Q_ARG(QObject*, parent)));
	if(!object)
		throw SerializerException(QStringLiteral("Failed to construct object of type %1").arg(metaObject->className()), true);

	//now deserialize all json properties
	for(auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++) {
		auto propIndex = metaObject->indexOfProperty(qUtf8Printable(it.key()));
		object->setProperty(qUtf8Printable(it.key()),
							doDeserialize(metaObject->property(propIndex), it.value(), object));
	}

	return object;
}
