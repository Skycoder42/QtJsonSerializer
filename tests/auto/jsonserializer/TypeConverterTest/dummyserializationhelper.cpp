#include "dummyserializationhelper.h"
#include <QtTest>
#include <QJsonSerializerException>
#include <QtJsonSerializer/private/qjsonexceptioncontext_p.h>

DummySerializationHelper::DummySerializationHelper(QObject *parent) :
	QObject{parent}
{}

QVariant DummySerializationHelper::getProperty(const char *name) const
{
	return properties.value(QString::fromUtf8(name));
}

QJsonValue DummySerializationHelper::serializeSubtype(QMetaProperty property, const QVariant &value) const
{
	return serializeSubtype(property.userType(), value, property.name());
}

QJsonValue DummySerializationHelper::serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx(propertyType, traceHint);
	if(serData.isEmpty())
		throw QJsonSerializationException{"No more data to serialize was expected"};

	auto data = serData.takeFirst();
	auto ok = false;
	[&](){
		QCOMPARE(propertyType, data.typeId);
		QCOMPARE(value, data.variant);
		ok = true;
	}();
	if(ok)
		return data.json;
	else
		throw QJsonSerializationException{"Data comparison failed"};
}

QVariant DummySerializationHelper::deserializeSubtype(QMetaProperty property, const QJsonValue &value, QObject *parent) const
{
	return deserializeSubtype(property.userType(), value, parent, property.name());
}

QVariant DummySerializationHelper::deserializeSubtype(int propertyType, const QJsonValue &value, QObject *parent, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx(propertyType, traceHint);
	if(deserData.isEmpty())
		throw QJsonDeserializationException{"No more data to deserialize was expected"};

	auto data = deserData.takeFirst();
	auto ok = false;
	[&](){
		QCOMPARE(propertyType, data.typeId);
		QCOMPARE(value, data.json);
		QCOMPARE(parent, this->parent());
		ok = true;
	}();
	if(ok)
		return data.variant;
	else
		throw QJsonDeserializationException{"Data comparison failed"};
}
