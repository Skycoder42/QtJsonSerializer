#include "qjsontypeconverter.h"

class QJsonTypeConverterPrivate
{
public:
	QJsonTypeConverterPrivate();

	int priority;
};

QJsonTypeConverter::QJsonTypeConverter() :
	d(new QJsonTypeConverterPrivate())
{}

QJsonTypeConverter::~QJsonTypeConverter() = default;

int QJsonTypeConverter::priority() const
{
	return d->priority;
}

void QJsonTypeConverter::setPriority(int priority)
{
	d->priority = priority;
}



QJsonTypeConverter::SerializationHelper::~SerializationHelper() = default;



QJsonValue QSimpleJsonTypeConverter::serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const
{
	Q_UNUSED(helper);
	return serialize(propertyType, value);
}

QVariant QSimpleJsonTypeConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper);
	return deserialize(propertyType, value, parent);
}



QJsonTypeConverterPrivate::QJsonTypeConverterPrivate() :
	priority(QJsonTypeConverter::Standard)
{}
