#include "qjsonenumconverter_p.h"
#include "qjsonserializerexception.h"
#include "qjsonserializer_p.h"

bool QJsonEnumConverter::canConvert(int metaTypeId) const
{
	return QMetaType::typeFlags(metaTypeId).testFlag(QMetaType::IsEnumeration);
}

QList<QJsonValue::Type> QJsonEnumConverter::jsonTypes() const
{
	return {
		QJsonValue::String,
		QJsonValue::Double //int only, but json does not have an int type
	};
}

QJsonValue QJsonEnumConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto d = QJsonSerializerPrivate::fromHelper(helper);
	if(d->enumAsString) {
		auto metaEnum = getMetaEnum<QJsonSerializationException>(propertyType);

		if(metaEnum.isFlag())
			return QString::fromUtf8(metaEnum.valueToKeys(value.toInt()));
		else
			return QString::fromUtf8(metaEnum.valueToKey(value.toInt()));
	} else
		return value.toInt();
}

QVariant QJsonEnumConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(parent)
	Q_UNUSED(helper)

	if(value.isString()) {
		auto metaEnum = getMetaEnum<QJsonDeserializationException>(propertyType);

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
		if(value.toDouble() == (double)intValue)
			return intValue;
		else
			throw QJsonDeserializationException("Invalid value (double) for enum type found: " + QByteArray::number(value.toDouble()));
	}
}

template<typename T>
QMetaEnum QJsonEnumConverter::getMetaEnum(int typeId)
{
	auto name = QMetaType::typeName(typeId);
	auto metaObject = QMetaType::metaObjectForType(typeId);
	if(!metaObject)
		throw T(QByteArray("Unable to get metaobject for type") + name);

	auto enumIndex = metaObject->indexOfEnumerator(name);
	if(enumIndex == -1)
		throw T(QByteArray("Unable to get meta enum for type") + name);
	return metaObject->enumerator(enumIndex);
}
