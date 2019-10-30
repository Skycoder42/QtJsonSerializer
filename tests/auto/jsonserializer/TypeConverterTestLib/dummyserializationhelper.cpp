#include "dummyserializationhelper.h"
#include <QtTest>
#include <QJsonSerializerException>
#include <QCborSerializer>

#include <QtJsonSerializer/private/qjsonserializerbase_p.h>
#include <QtJsonSerializer/private/qjsonexceptioncontext_p.h>

DummySerializationHelper::DummySerializationHelper(QObject *parent) :
	QObject{parent}
{}

bool DummySerializationHelper::jsonMode() const
{
	return json;
}

QVariant DummySerializationHelper::getProperty(const char *name) const
{
	return properties.value(QString::fromUtf8(name));
}

QCborTag DummySerializationHelper::typeTag(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return properties.value(QStringLiteral("typeTag"),
							QVariant::fromValue(static_cast<QCborTag>(QCborSerializer::NoTag)))
			.value<QCborTag>();
}

QSharedPointer<const QJsonTypeExtractor> DummySerializationHelper::extractor(int metaTypeId) const
{
	return QJsonSerializerBasePrivate::extractors.get(metaTypeId);
}

QCborValue DummySerializationHelper::serializeSubtype(const QMetaProperty &property, const QVariant &value) const
{
	return serializeSubtype(property.userType(), value, property.name());
}

QCborValue DummySerializationHelper::serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx{propertyType, traceHint};
	if (serData.isEmpty())
		throw QJsonSerializationException{"No more data to serialize was expected"};

	for (auto i = 0; i < serData.size(); ++i) {
		if (serData[i].typeId != propertyType)
			continue;

		auto data = serData.takeAt(i);
		auto ok = false;
		[&](){
			QCOMPARE(value, data.variant);
			ok = true;
		}();
		if (ok)
			return data.cbor;
		else
			throw QJsonSerializationException{"Data comparison failed"};
	}

	throw QJsonSerializationException{QByteArrayLiteral("Unable to find data of type ") + QMetaType::typeName(propertyType) + QByteArrayLiteral(" in serData")};
}

QVariant DummySerializationHelper::deserializeSubtype(const QMetaProperty &property, const QCborValue &value, QObject *parent) const
{
	return deserializeSubtype(property.userType(), value, parent, property.name());
}

QVariant DummySerializationHelper::deserializeSubtype(int propertyType, const QCborValue &value, QObject *parent, const QByteArray &traceHint) const
{
	QJsonExceptionContext ctx{propertyType, traceHint};
	if (deserData.isEmpty())
		throw QJsonDeserializationException{"No more data to deserialize was expected"};

	for(auto i = 0; i < deserData.size(); i++) {
		if (deserData[i].typeId != propertyType)
			continue;

		auto data = deserData.takeAt(i);
		auto ok = false;
		[&](){
			QCOMPARE(value, data.cbor);
			if(expectedParent)
				QCOMPARE(parent, expectedParent);
			ok = true;
		}();
		if (ok)
			return data.variant;
		else
			throw QJsonDeserializationException{"Data comparison failed"};
	}

	throw QJsonDeserializationException{QByteArrayLiteral("Unable to find data of type ") + QMetaType::typeName(propertyType) + QByteArrayLiteral(" in deserData")};
}
