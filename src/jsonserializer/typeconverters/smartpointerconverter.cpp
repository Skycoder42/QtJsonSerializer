#include "smartpointerconverter_p.h"
#include "exception.h"
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool SmartPointerConverter::canConvert(int metaTypeId) const
{
	const auto extractor = helper()->extractor(metaTypeId);
	return extractor && (extractor->baseType() == "pointer" ||
						 extractor->baseType() == "qpointer");
}

QList<QCborValue::Type> SmartPointerConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
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

QCborValue SmartPointerConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw SerializationException(QByteArray("Failed to get extractor for type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(". Make shure to register std::optional types via QJsonSerializer::registerPointerConverters"));
	}

	return helper()->serializeSubtype(extractor->subtypes()[0],
									  extractor->extract(value),
									  "data");
}

QVariant SmartPointerConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto extractor = helper()->extractor(propertyType);
	if (!extractor) {
		throw DeserializationException(QByteArray("Failed to get extractor for type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(". Make shure to register std::optional types via QJsonSerializer::registerPointerConverters"));
	}

	auto result = helper()->deserializeSubtype(extractor->subtypes()[0],
											   value,
											   extractor->baseType() == "qpointer" ? parent : nullptr,
											   "data");
	extractor->emplace(result, result);
	return result;
}
