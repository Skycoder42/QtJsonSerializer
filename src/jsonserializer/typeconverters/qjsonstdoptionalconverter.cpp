#include "qjsonstdoptionalconverter_p.h"
#include "qjsonserializerexception.h"

#include <optional>

// WORKAROUND for now, nullptr is used instead of nullopt, as nullopt_t cannot be registered as metatype

const QRegularExpression QJsonStdOptionalConverter::optionalTypeRegex(QStringLiteral(R"__(^std::optional<\s*(.*?)\s*>$)__"));

bool QJsonStdOptionalConverter::canConvert(int metaTypeId) const
{
	return optionalTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QJsonValue::Type> QJsonStdOptionalConverter::jsonTypes() const
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

QJsonValue QJsonStdOptionalConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto cValue = value;
	if(!cValue.convert(QMetaType::QVariant)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to a QVariant. Make shure to register optional types via QJsonSerializer::registerOptionalConverters"));
	}
	cValue = cValue.value<QVariant>();

	if (cValue.userType() == QMetaType::Nullptr)
		return QJsonValue::Null;
	else {
		const auto metaType = getSubtype(propertyType);
		if (cValue.userType() != metaType) {
			throw QJsonSerializationException(QByteArray("Invalid value given for type ") +
											  QMetaType::typeName(propertyType) +
											  QByteArray(" - was ") +
											  value.typeName() +
											  QByteArray(", which is not the optionals value type (") +
											  QMetaType::typeName(metaType) +
											  QByteArray(")"));
		}
		return helper->serializeSubtype(metaType, cValue, "value");
	}
}

QVariant QJsonStdOptionalConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	QVariant result;
	if (value.isNull())
		result = QVariant::fromValue(nullptr);
	else
		result = helper->deserializeSubtype(getSubtype(propertyType), value, parent, "value");
	return QVariant{QMetaType::QVariant, &result};
}

int QJsonStdOptionalConverter::getSubtype(int optionalType) const
{
	auto match = optionalTypeRegex.match(QString::fromUtf8(getCanonicalTypeName(optionalType)));
	if(match.hasMatch())
		return QMetaType::type(match.captured(1).toUtf8().trimmed());
	else
		return QMetaType::UnknownType;
}
