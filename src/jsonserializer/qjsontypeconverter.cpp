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



QJsonTypeConverterPrivate::QJsonTypeConverterPrivate() :
	priority(QJsonTypeConverter::Standard)
{}
