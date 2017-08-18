#include "qjsonmapconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QJsonObject>

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 2)
const QRegularExpression QJsonMapConverter::mapTypeRegex(QStringLiteral(R"__(^(?:QMap|QHash)<\s*QString\s*,\s*(.*?)\s*>$)__"));
#else
const QRegularExpression QJsonMapConverter::mapTypeRegex(QStringLiteral(R"__(^QMap<\s*QString\s*,\s*(.*?)\s*>$)__"));
#endif

bool QJsonMapConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QVariantMap ||
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 2)
			QMetaType::QVariantHash ||
#endif
			mapTypeRegex.match(QString::fromUtf8(QMetaType::typeName(metaTypeId))).hasMatch();
}

QList<QJsonValue::Type> QJsonMapConverter::jsonTypes() const
{
	return {QJsonValue::Object};
}

QJsonValue QJsonMapConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto metaType = getSubtype(propertyType);

	auto cValue = value;
	if(!cValue.convert(QVariant::Map)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to a variant map. Make shure to register map types via QJsonSerializer::registerMapConverters"));
	}
	auto map = cValue.toMap();

	QJsonObject object;
	for(auto it = map.constBegin(); it != map.constEnd(); ++it)
		object.insert(it.key(), helper->serializeSubtype(metaType, it.value(), it.key().toUtf8()));
	return object;
}

QVariant QJsonMapConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto metaType = getSubtype(propertyType);

	//generate the map
	QVariantMap map;
	auto object = value.toObject();
	for(auto it = object.constBegin(); it != object.constEnd(); ++it)
		map.insert(it.key(), helper->deserializeSubtype(metaType, it.value(), parent, it.key().toUtf8()));
	return map;
}

int QJsonMapConverter::getSubtype(int mapType)
{
	int metaType = QMetaType::UnknownType;
	auto match = mapTypeRegex.match(QString::fromUtf8(QMetaType::typeName(mapType)));
	if(match.hasMatch())
		metaType = QMetaType::type(match.captured(1).toUtf8().trimmed());
	return metaType;
}
