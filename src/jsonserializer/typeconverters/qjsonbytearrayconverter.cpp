#include "qjsonbytearrayconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QByteArray>

bool QJsonBytearrayConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QByteArray;
}

QList<QJsonValue::Type> QJsonBytearrayConverter::jsonTypes() const
{
	return {QJsonValue::String};
}

QJsonValue QJsonBytearrayConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(helper)

	return QString::fromUtf8(value.toByteArray().toBase64());
}

QVariant QJsonBytearrayConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	Q_UNUSED(helper)

	return QByteArray::fromBase64(value.toString().toUtf8());
}
