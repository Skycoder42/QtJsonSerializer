#include "qjsontypeconverter.h"

class QJsonTypeConverterPrivate
{
public:
	int priority = QJsonTypeConverter::Standard;
};

QJsonTypeConverter::QJsonTypeConverter() :
	d{new QJsonTypeConverterPrivate{}}
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



QJsonTypeConverter::SerializationHelper::SerializationHelper() = default;

QJsonTypeConverter::SerializationHelper::~SerializationHelper() = default;
