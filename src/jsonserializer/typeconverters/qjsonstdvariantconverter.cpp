#include "qjsonstdvariantconverter_p.h"
#include "qjsonserializerexception.h"
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool QJsonStdVariantConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && extractor->baseType() == "variant";
}

QList<QCborValue::Type> QJsonStdVariantConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	QList<QCborValue::Type> types;
	const auto metaEnum = QMetaEnum::fromType<QCborValue::Type>();
	types.reserve(metaEnum.keyCount());
	for (auto i = 0; i < metaEnum.keyCount(); ++i) {
		if (const auto value = metaEnum.value(i); value != QCborValue::Invalid)
			types.append(static_cast<QCborValue::Type>(value));
	}
	return types;
}

QCborValue QJsonStdVariantConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw QJsonSerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(". Make shure to register std::variant types via QJsonSerializer::registerVariantConverters"));
	}

	const auto metaTypes = extractor->subtypes();
	const auto cValue = extractor->extract(value);
	const auto mIndex = metaTypes.indexOf(cValue.userType());
	if (mIndex == -1) {
		throw QJsonSerializationException(QByteArray("Invalid value given for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" - was ") +
										  value.typeName() +
										  QByteArray(", which is not a type of the given variant"));
	}
	return helper()->serializeSubtype(metaTypes[mIndex], cValue, QByteArray{"<"} + QMetaType::typeName(metaTypes[mIndex]) + QByteArray{">"});
}

QVariant QJsonStdVariantConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw QJsonDeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(". Make shure to register std::variant types via QJsonSerializer::registerVariantConverters"));
	}

	// try all types until one succeeds
	for (auto metaType : extractor->subtypes()) {
		// ignore exceptions and try with the next type
		try {
			auto result = helper()->deserializeSubtype(metaType, value, parent, QByteArray{"<"} + QMetaType::typeName(metaType) + QByteArray{">"});
			extractor->emplace(result, result);
			return result;
		} catch (QJsonDeserializationException &) {}
	}

	throw QJsonDeserializationException(QByteArray("Failed to deserialze value to ") +
										QMetaType::typeName(propertyType) +
										QByteArray(" because all possible sub-type converters rejected the passed value."));
}
