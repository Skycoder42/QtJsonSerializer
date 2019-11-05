#include "qjsonenumconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <cmath>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

namespace {

Q_NORETURN inline void throwSer(QByteArray &&what, bool ser)
{
	if (ser)
		throw QJsonSerializationException{std::move(what)};
	else
		throw QJsonDeserializationException{std::move(what)};
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

QList<QCborTag> QJsonEnumConverter::allowedCborTags(int metaTypeId) const
{
	const auto metaEnum = getEnum(metaTypeId, false);
	if (metaEnum.isFlag())
		return {static_cast<QCborTag>(QCborSerializer::Flags)};
	else
		return {static_cast<QCborTag>(QCborSerializer::Enum)};
}

QList<QCborValue::Type> QJsonEnumConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Integer, QCborValue::String};
}

QCborValue QJsonEnumConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto metaEnum = getEnum(propertyType, true);
	const auto tag = static_cast<QCborTag>(metaEnum.isFlag() ? QCborSerializer::Flags : QCborSerializer::Enum);
	if (helper()->getProperty("enumAsString").toBool()) {
		if (metaEnum.isFlag())
			return {tag, QString::fromUtf8(metaEnum.valueToKeys(value.toInt()))};
		else
			return {tag, QString::fromUtf8(metaEnum.valueToKey(value.toInt()))};
	} else
		return {tag, value.toInt()};
}

QVariant QJsonEnumConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(parent)
	const auto metaEnum = getEnum(propertyType, false);
	auto cValue = value.isTag() ? value.taggedValue() : value;
	if (cValue.isString()) {
		auto result = -1;
		auto ok = false;
		if (metaEnum.isFlag())
			result = metaEnum.keysToValue(qUtf8Printable(cValue.toString()), &ok);
		else
			result = metaEnum.keyToValue(qUtf8Printable(cValue.toString()), &ok);
		if (ok)
			return result;
		else if(metaEnum.isFlag() && cValue.toString().isEmpty())
			return 0;
		else {
			throw QJsonDeserializationException{QByteArray{"Invalid value for enum type \""} +
												metaEnum.name() +
												"\": " +
												cValue.toString().toUtf8()};
		}
	} else {
		const auto intValue = cValue.toInteger();
		if (!metaEnum.isFlag() && metaEnum.valueToKey(intValue) == nullptr) {
			throw QJsonDeserializationException{"Invalid integer value. Not a valid enum/flags element: " +
												QByteArray::number(intValue)};
		}
		return static_cast<int>(intValue);
	}
}

QVariant QJsonEnumConverter::deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const
{
	if (value.isDouble()) {
		double intpart;
		if (std::modf(value.toDouble(), &intpart) != 0.0) {
			throw QJsonDeserializationException{"Invalid value (double) for enum type found: " +
												QByteArray::number(value.toDouble())};
		}
	}
	return deserializeCbor(propertyType, value, parent);
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
