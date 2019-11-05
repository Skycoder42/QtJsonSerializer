#include "qjsonstdoptionalconverter_p.h"
#include "qjsonserializerexception.h"

#include <optional>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

// WORKAROUND for now, nullptr is used instead of nullopt, as nullopt_t cannot be registered as metatype

bool QJsonStdOptionalConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && extractor->baseType() == "optional";
}

QList<QCborValue::Type> QJsonStdOptionalConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
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

QCborValue QJsonStdOptionalConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw QJsonSerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(". Make shure to register std::optional types via QJsonSerializer::registerOptionalConverters"));
	}

	const auto cValue = extractor->extract(value);
	if (cValue.userType() == QMetaType::Nullptr)
		return QCborValue::Null;
	else
		return helper()->serializeSubtype(extractor->subtypes()[0], cValue, "value");
}

QVariant QJsonStdOptionalConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw QJsonDeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaType::typeName(propertyType) +
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
