#include "qjsonpairconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QJsonArray>

const QRegularExpression QJsonPairConverter::pairTypeRegex(QStringLiteral(R"__(^QPair<\s*(.*?)\s*,\s*(.*?)\s*>$)__"));

bool QJsonPairConverter::canConvert(int metaTypeId) const
{
	return pairTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QJsonValue::Type> QJsonPairConverter::jsonTypes() const
{
	return {QJsonValue::Array};
}

QJsonValue QJsonPairConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto types = getPairTypes(propertyType);
	auto targetType = qMetaTypeId<QPair<QVariant, QVariant>>();
	auto cValue = value;
	if(!cValue.canConvert(targetType) || !cValue.convert(targetType)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to QPair<QVariant, QVariant>. Make shure to register pair types via QJsonSerializer::registerPairConverters"));
	}

	auto variant = cValue.value<QPair<QVariant, QVariant>>();
	QJsonArray array;
	array.append(helper->serializeSubtype(types.first, variant.first, "pair.first"));
	array.append(helper->serializeSubtype(types.second, variant.second, "pair.second"));
	return array;
}

QVariant QJsonPairConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto types = getPairTypes(propertyType);
	auto array = value.toArray();
	if(array.size() != 2)
		throw QJsonDeserializationException("Json array must have exactly 2 elements to be read as a pair");

	QPair<QVariant, QVariant> vPair;
	vPair.first = helper->deserializeSubtype(types.first, array[0], parent, "pair.first");
	vPair.second = helper->deserializeSubtype(types.second, array[1], parent, "pair.second");
	return QVariant::fromValue(vPair);
}

QPair<int, int> QJsonPairConverter::getPairTypes(int metaType) const
{
	auto match = pairTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(metaType)));
	if(match.hasMatch()) {
		QPair<int, int> types;
		types.first = QMetaType::type(match.captured(1).toUtf8().trimmed());
		types.second = QMetaType::type(match.captured(2).toUtf8().trimmed());
		return types;
	} else
		return {QMetaType::UnknownType, QMetaType::UnknownType};
}
