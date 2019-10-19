#include "testconverter.h"
#include <QCborMap>

bool EnumContainer::operator==(EnumContainer other) const
{
	return normalEnum == other.normalEnum &&
		   enumFlags == other.enumFlags;
}

bool EnumContainer::operator!=(EnumContainer other) const
{
	return normalEnum != other.normalEnum ||
		   enumFlags != other.enumFlags;
}

EnumContainer::EnumFlags EnumContainer::getEnumFlags() const
{
	return enumFlags;
}

void EnumContainer::setEnumFlags(EnumFlags value)
{
	enumFlags = value;
}



TestEnumConverter::TestEnumConverter()
{
	setPriority(Priority::VeryHigh);
}

bool TestEnumConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == qMetaTypeId<NormalEnum>() ||
			metaTypeId == qMetaTypeId<EnumFlags>();
}

QList<QCborValue::Type> TestEnumConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::String};
}

QCborValue TestEnumConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper)
	if (propertyType == qMetaTypeId<NormalEnum>()) {
		const auto me = QMetaEnum::fromType<NormalEnum>();
		return QString::fromUtf8(me.valueToKey(value.toInt()));
	} else {
		const auto me = QMetaEnum::fromType<EnumFlags>();
		return QString::fromUtf8(me.valueToKeys(value.toInt()));
	}
}

QVariant TestEnumConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(parent)
	Q_UNUSED(helper)
	if (propertyType == qMetaTypeId<NormalEnum>()) {
		const auto me = QMetaEnum::fromType<NormalEnum>();
		return me.keyToValue(qUtf8Printable(value.toString()));
	} else {
		const auto me = QMetaEnum::fromType<EnumFlags>();
		return me.keysToValue(qUtf8Printable(value.toString()));
	}
}



TestWrapperConverter::TestWrapperConverter()
{
	setPriority(Priority::VeryHigh);
}

bool TestWrapperConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == qMetaTypeId<EnumContainer>();
}

QList<QCborValue::Type> TestWrapperConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Map};
}

QCborValue TestWrapperConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto mo = &EnumContainer::staticMetaObject;
	auto container = value.value<EnumContainer>();
	return QCborMap {
		{QStringLiteral("0"), helper->serializeSubtype(mo->property(mo->propertyOffset()), static_cast<int>(container.normalEnum))},
		{QStringLiteral("1"), helper->serializeSubtype(mo->property(mo->propertyOffset() + 1), static_cast<int>(container.enumFlags))}
	};
}

QVariant TestWrapperConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto mo = &EnumContainer::staticMetaObject;
	auto map = value.toMap();
	if (map.contains(QStringLiteral("error")))
		helper->deserializeSubtype(propertyType, QCborValue{42}, parent, "test");
	return QVariant::fromValue(EnumContainer {
		static_cast<EnumContainer::NormalEnum>(helper->deserializeSubtype(mo->property(mo->propertyOffset()), map.value(QStringLiteral("0")), parent).toInt()),
		static_cast<EnumContainer::EnumFlags>(helper->deserializeSubtype(mo->property(mo->propertyOffset() + 1), map.value(QStringLiteral("1")), parent).toInt())
	});
}



TestObject::TestObject(QObject *parent)
	: QObject{parent}
{}
