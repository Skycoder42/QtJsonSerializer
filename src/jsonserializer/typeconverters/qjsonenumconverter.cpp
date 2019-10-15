#include "qjsonenumconverter_p.h"
#include "qjsonserializerexception.h"

#include <cmath>

namespace {

Q_NORETURN inline void throwSer(QByteArray &&what, bool ser)
{
	if (ser)
		throw QJsonSerializationException{std::move(what)};
	else
		throw QJsonDeserializationException{std::move(what)};
}

}

QCborValue QJsonEnumConverter::serializeEnum(const QMetaEnum &metaEnum, const QVariant &value, bool enumAsString)
{
	// TODO fix
	if (enumAsString) {
		if(metaEnum.isFlag())
			return QString::fromUtf8(metaEnum.valueToKeys(value.toInt()));
		else
			return QString::fromUtf8(metaEnum.valueToKey(value.toInt()));
	} else
		return value.toInt();
}

QVariant QJsonEnumConverter::deserializeEnum(const QMetaEnum &metaEnum, const QCborValue &value)
{
	// TODO fix
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
		else {
			throw QJsonDeserializationException{QByteArray{"Invalid value for enum type \""} +
												metaEnum.name() +
												"\": " +
												value.toString().toUtf8()};
		}
	} else {
		const auto intValue = value.toInteger();
		double intpart;
		if(std::modf(value.toDouble(), &intpart) != 0.0) {
			throw QJsonDeserializationException{"Invalid value (double) for enum type found: " +
												QByteArray::number(value.toDouble())};
		}
		if(!metaEnum.isFlag() && metaEnum.valueToKey(intValue) == nullptr) {
			throw QJsonDeserializationException{"Invalid integer value. Not a valid enum element: " +
												QByteArray::number(intValue)};
		}
		return intValue;
	}
}

QJsonEnumConverter::QJsonEnumConverter()
{
	setPriority(QJsonTypeConverter::Low);
}

bool QJsonEnumConverter::canConvert(int metaTypeId) const
{
	return QMetaType::typeFlags(metaTypeId).testFlag(QMetaType::IsEnumeration) ||
			testForEnum(metaTypeId);  // NOTE check once in a while if still needed
}

QList<QJsonValue::Type> QJsonEnumConverter::jsonTypes() const
{
	return {QJsonValue::Double, QJsonValue::String};
}

QJsonValue QJsonEnumConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	return serializeEnum(getEnum(propertyType, true),
						 value,
						 helper->getProperty("enumAsString").toBool());
}

QVariant QJsonEnumConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(parent)
	Q_UNUSED(helper)
	return deserializeEnum(getEnum(propertyType, true), value);
}

bool QJsonEnumConverter::testForEnum(int metaTypeId) const
{
	try {
		getEnum(metaTypeId, true);
		return true;
	} catch (QJsonSerializerException &) {
		return false;
	}
}

QMetaEnum QJsonEnumConverter::getEnum(int metaTypeId, bool ser) const
{
	const auto mo = QMetaType::metaObjectForType(metaTypeId);
	if (!mo)
		throwSer(QByteArray{"Unable to get metaobject for type "} + QMetaType::typeName(metaTypeId), ser);
	const auto enumName = QString::fromUtf8(QMetaType::typeName(metaTypeId))
							  .split(QStringLiteral("::"))
							  .last()
							  .toUtf8();
	auto mIndex = mo->indexOfEnumerator(enumName.data());
	if (mIndex < 0) {
		throwSer(QByteArray{"Unable to get QMetaEnum for type "} +
					 QMetaType::typeName(metaTypeId) +
					 QByteArray{" using the owning meta object "} +
					 mo->className(),
				 ser);
	}

	return mo->enumerator(mIndex);
}
