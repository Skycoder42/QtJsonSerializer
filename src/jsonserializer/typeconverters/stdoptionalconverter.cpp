#include "stdoptionalconverter_p.h"
#include "exception.h"

#include <optional>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

// WORKAROUND for now, nullptr is used instead of nullopt, as nullopt_t cannot be registered as metatype

bool StdOptionalConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && extractor->baseType() == "optional";
}

QList<QCborValue::Type> StdOptionalConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
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

QCborValue StdOptionalConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw SerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaTypeName(propertyType) +
										  QByteArray(". Make shure to register std::optional types via QJsonSerializer::registerOptionalConverters"));
	}

	const auto cValue = extractor->extract(value);
	if (cValue.userType() == QMetaType::Nullptr)
		return QCborValue::Null;
	else
		return helper()->serializeSubtype(extractor->subtypes()[0], cValue, "value");
}

QVariant StdOptionalConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw DeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaTypeName(propertyType) +
											QByteArray(". Make shure to register std::optional types via QJsonSerializer::registerOptionalConverters"));
	}

	QVariant result;
	if (value.isNull())
		result = QVariant::fromValue(nullptr);
	else
		result = helper()->deserializeSubtype(extractor->subtypes()[0], value, parent, "value");
	extractor->emplace(result, result);
	return result;
}
