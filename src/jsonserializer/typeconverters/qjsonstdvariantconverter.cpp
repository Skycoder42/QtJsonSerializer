#include "qjsonstdvariantconverter_p.h"
#include "qjsonserializerexception.h"

const QRegularExpression QJsonStdVariantConverter::variantTypeRegex(QStringLiteral(R"__(^std::variant<(\s*.*?\s*(?:,\s*.*?\s*)*)>$)__"));

bool QJsonStdVariantConverter::canConvert(int metaTypeId) const
{
	return variantTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QJsonValue::Type> QJsonStdVariantConverter::jsonTypes() const
{
	// allow all values
	return {
		QJsonValue::Null,
		QJsonValue::Bool,
		QJsonValue::Double,
		QJsonValue::String,
		QJsonValue::Array,
		QJsonValue::Object
	};
}

QJsonValue QJsonStdVariantConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto cValue = value;
	if(!cValue.convert(QMetaType::QVariant)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to a QVariant. Make shure to register variant types via QJsonSerializer::registerVariantConverters"));
	}
	cValue = cValue.value<QVariant>();

	const auto metaTypes = getSubtypes(propertyType);
	const auto mIndex = metaTypes.indexOf(cValue.userType());
	if (mIndex == -1) {
		throw QJsonSerializationException(QByteArray("Invalid value given for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" - was ") +
										  value.typeName() +
										  QByteArray(", which is not a type of the given variant"));
	}
	return helper->serializeSubtype(metaTypes[mIndex], cValue, QByteArray{"<"} + QMetaType::typeName(metaTypes[mIndex]) + QByteArray{">"});
}

QVariant QJsonStdVariantConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	// try all types until one succeeds
	for (auto metaType : getSubtypes(propertyType)) {
		// ignore exceptions and try with the next type
		try {
			auto result = helper->deserializeSubtype(metaType, value, parent, QByteArray{"<"} + QMetaType::typeName(metaType) + QByteArray{">"});
			return QVariant{QMetaType::QVariant, &result};
		} catch (QJsonDeserializationException &) {}
	}

	throw QJsonDeserializationException(QByteArray("Failed to deserialze value to ") +
										QMetaType::typeName(propertyType) +
										QByteArray(" because all possible sub-type converters rejected the passed value."));
}

QList<int> QJsonStdVariantConverter::getSubtypes(int metaType) const
{
	auto match = variantTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(metaType)));
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
