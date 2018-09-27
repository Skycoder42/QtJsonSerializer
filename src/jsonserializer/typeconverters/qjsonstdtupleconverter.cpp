#include "qjsonstdtupleconverter_p.h"

#include <QtCore/QJsonArray>

#include "qjsonserializerexception.h"

const QRegularExpression QJsonStdTupleConverter::tupleTypeRegex(QStringLiteral(R"__(^std::tuple<(\s*.*?\s*(?:,\s*.*?\s*)*)>$)__"));

bool QJsonStdTupleConverter::canConvert(int metaTypeId) const
{
	return tupleTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QJsonValue::Type> QJsonStdTupleConverter::jsonTypes() const
{
	return {QJsonValue::Array};
}

QJsonValue QJsonStdTupleConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	const auto types = getSubtypes(propertyType);
	if(types.isEmpty())
		throw QJsonSerializationException{QByteArray{"Failed to extract element types from "} + QMetaType::typeName(propertyType)};

	auto cValue = value;
	if(!cValue.canConvert(QMetaType::QVariantList) || !cValue.convert(QMetaType::QVariantList)) {
		throw QJsonSerializationException {
			QByteArray{"Failed to convert type "} +
			QMetaType::typeName(propertyType) +
			QByteArray{" to a variant list. Make shure to register tuple types via QJsonSerializer::registerTupleConverters"}
		};
	}

	const auto vList = cValue.toList();
	if(vList.size() != types.size()) {
		throw QJsonSerializationException {
			QByteArray{"Element count mismatch on value for type "} +
			QMetaType::typeName(propertyType) +
			QByteArray{". Detected "} + QByteArray::number(types.size()) + QByteArray{" types for the tuple, but the passed value has "} +
			QByteArray::number(vList.size()) + QByteArray{" elements"}
		};
	}

	QJsonArray array;
	for(auto i = 0, max = vList.size(); i < max; ++i)
		array.append(helper->serializeSubtype(types[i], vList[i], "<" + QByteArray::number(i) + ">"));
	return array;
}

QVariant QJsonStdTupleConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	const auto types = getSubtypes(propertyType);
	if(types.isEmpty())
		throw QJsonSerializationException{QByteArray{"Failed to extract element types from "} + QMetaType::typeName(propertyType)};

	const auto array = value.toArray();
	if(array.size() != types.size()) {
		throw QJsonDeserializationException {
			QByteArray{"Element count mismatch on value for type "} +
			QMetaType::typeName(propertyType) +
			QByteArray{". Detected "} + QByteArray::number(types.size()) + QByteArray{" types for the tuple, but the json array has "} +
			QByteArray::number(array.size()) + QByteArray{" elements"}
		};
	}

	QVariantList list;
	list.reserve(array.size());
	for(auto i = 0, max = array.size(); i < max; ++i)
		list.append(helper->deserializeSubtype(types[i], array[i], parent, "<" + QByteArray::number(i) + ">"));
	return list;
}

QList<int> QJsonStdTupleConverter::getSubtypes(int metaType) const
{
	auto match = tupleTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(metaType)));
	if(match.hasMatch()) {
		QList<int> types;
		const auto typeNames = match.captured(1).split(QLatin1Char(','));
		types.reserve(typeNames.size());
		for(const auto &name : typeNames)
			types.append(QMetaType::type(name.toUtf8().trimmed()));
		return types;
	} else
		return {};
}
