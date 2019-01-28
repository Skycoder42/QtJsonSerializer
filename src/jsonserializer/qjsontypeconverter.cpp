#include "qjsontypeconverter.h"
#include "qjsonserializer_p.h"

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

QByteArray QJsonTypeConverter::getCanonicalTypeName(int propertyType) const
{
	return QJsonSerializerPrivate::getTypeName(propertyType);
}



QJsonTypeConverter::SerializationHelper::SerializationHelper() = default;

QJsonTypeConverter::SerializationHelper::~SerializationHelper() = default;



QJsonTypeConverterFactory::QJsonTypeConverterFactory() = default;

QJsonTypeConverterFactory::~QJsonTypeConverterFactory() = default;

int QJsonTypeConverterFactory::priority() const
{
	if(!_statusConverter)
		_statusConverter = createConverter();
	return _statusConverter->priority();
}

bool QJsonTypeConverterFactory::canConvert(int metaTypeId) const
{
	if(!_statusConverter)
		_statusConverter = createConverter();
	return _statusConverter->canConvert(metaTypeId);
}

QList<QJsonValue::Type> QJsonTypeConverterFactory::jsonTypes() const
{
	if(!_statusConverter)
		_statusConverter = createConverter();
	return _statusConverter->jsonTypes();
}
