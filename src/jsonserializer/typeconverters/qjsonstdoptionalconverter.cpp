#include "qjsonstdoptionalconverter_p.h"
#include "qjsonserializerexception.h"

#include <optional>

// WORKAROUND for now, nullptr is used instead of nullopt, as nullopt_t cannot be registered as metatype

const QRegularExpression QJsonStdOptionalConverter::optionalTypeRegex(QStringLiteral(R"__(^std::optional<\s*(.*?)\s*>$)__"));

bool QJsonStdOptionalConverter::canConvert(int metaTypeId) const
{
	return optionalTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(metaTypeId))).hasMatch();
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
	auto cValue = value;
	if (!cValue.convert(QMetaType::QVariant)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to a QVariant. Make shure to register optional types via QJsonSerializer::registerOptionalConverters"));
	}
	cValue = cValue.value<QVariant>();

	if (cValue.userType() == QMetaType::Nullptr)
		return QCborValue::Null;
	else
		return helper()->serializeSubtype(getSubtype(propertyType), cValue, "value");
}

QVariant QJsonStdOptionalConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	QVariant result;
	if (value.isNull())
		result = QVariant::fromValue(nullptr);
	else
		result = helper()->deserializeSubtype(getSubtype(propertyType), value, parent, "value");
	return QVariant{QMetaType::QVariant, &result};
}

int QJsonStdOptionalConverter::getSubtype(int optionalType) const
{
	auto match = optionalTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(optionalType)));
	if (match.hasMatch())
		return QMetaType::type(match.captured(1).toUtf8().trimmed());
	else
		return QMetaType::UnknownType;
}
