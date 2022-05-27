#include "stdvariantconverter_p.h"
#include "exception.h"
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool StdVariantConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && extractor->baseType() == "variant";
}

QList<QCborValue::Type> StdVariantConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
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

QCborValue StdVariantConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw SerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaTypeName(propertyType) +
										  QByteArray(". Make shure to register std::variant types via QJsonSerializer::registerVariantConverters"));
	}

	const auto metaTypes = extractor->subtypes();
	const auto cValue = extractor->extract(value);
	const auto mIndex = metaTypes.indexOf(cValue.userType());
	if (mIndex == -1) {
		throw SerializationException(QByteArray("Invalid value given for type ") +
										  QMetaTypeName(propertyType) +
										  QByteArray(" - was ") +
										  value.typeName() +
										  QByteArray(", which is not a type of the given variant"));
	}
	return helper()->serializeSubtype(metaTypes[mIndex], cValue, QByteArray{"<"} + QMetaTypeName(metaTypes[mIndex]) + QByteArray{">"});
}

QVariant StdVariantConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw DeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaTypeName(propertyType) +
											QByteArray(". Make shure to register std::variant types via QJsonSerializer::registerVariantConverters"));
	}

	// try all types until one succeeds
	for (auto metaType : extractor->subtypes()) {
		// ignore exceptions and try with the next type
		try {
			auto result = helper()->deserializeSubtype(metaType, value, parent, QByteArray{"<"} + QMetaTypeName(metaType) + QByteArray{">"});
			extractor->emplace(result, result);
			return result;
		} catch (DeserializationException &) {}
	}

	throw DeserializationException(QByteArray("Failed to deserialze value to ") +
										QMetaTypeName(propertyType) +
										QByteArray(" because all possible sub-type converters rejected the passed value."));
}
