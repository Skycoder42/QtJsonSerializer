#include "enumconverter_p.h"
#include "exception.h"
#include "cborserializer.h"

#include <cmath>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

namespace {

Q_NORETURN inline void throwSer(QByteArray &&what, bool ser)
{
	if (ser)
		throw SerializationException{std::move(what)};
	else
		throw DeserializationException{std::move(what)};
}

}

EnumConverter::EnumConverter()
{
	setPriority(TypeConverter::Low);
}

bool EnumConverter::canConvert(int metaTypeId) const
{
	return QMetaType::typeFlags(metaTypeId).testFlag(QMetaType::IsEnumeration) ||
			testForEnum(metaTypeId);  // NOTE check once in a while if still needed
}

QList<QCborTag> EnumConverter::allowedCborTags(int metaTypeId) const
{
	const auto metaEnum = getEnum(metaTypeId, false);
	if (metaEnum.isFlag())
		return {static_cast<QCborTag>(CborSerializer::Flags)};
	else
		return {static_cast<QCborTag>(CborSerializer::Enum)};
}

QList<QCborValue::Type> EnumConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Integer, QCborValue::String};
}

QCborValue EnumConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto metaEnum = getEnum(propertyType, true);
	const auto tag = static_cast<QCborTag>(metaEnum.isFlag() ? CborSerializer::Flags : CborSerializer::Enum);
	if (helper()->getProperty("enumAsString").toBool()) {
		if (metaEnum.isFlag())
			return {tag, QString::fromUtf8(metaEnum.valueToKeys(value.toInt()))};
		else
			return {tag, QString::fromUtf8(metaEnum.valueToKey(value.toInt()))};
	} else
		return {tag, value.toInt()};
}

QVariant EnumConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
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
			throw DeserializationException{QByteArray{"Invalid value for enum type \""} +
												metaEnum.name() +
												"\": " +
												cValue.toString().toUtf8()};
		}
	} else {
		const auto intValue = cValue.toInteger();
		if (!metaEnum.isFlag() && metaEnum.valueToKey(intValue) == nullptr) {
			throw DeserializationException{"Invalid integer value. Not a valid enum/flags element: " +
												QByteArray::number(intValue)};
		}
		return static_cast<int>(intValue);
	}
}

QVariant EnumConverter::deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const
{
	if (value.isDouble()) {
		double intpart;
		if (std::modf(value.toDouble(), &intpart) != 0.0) {
			throw DeserializationException{"Invalid value (double) for enum type found: " +
												QByteArray::number(value.toDouble())};
		}
	}
	return deserializeCbor(propertyType, value, parent);
}

bool EnumConverter::testForEnum(int metaTypeId) const
{
	try {
		getEnum(metaTypeId, true);
		return true;
	} catch (Exception &) {
		return false;
	}
}

QMetaEnum EnumConverter::getEnum(int metaTypeId, bool ser) const
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
