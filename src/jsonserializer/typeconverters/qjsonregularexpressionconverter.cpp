#include "qjsonregularexpressionconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QJsonObject>

bool QJsonRegularExpressionConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QRegularExpression;
}

QList<QJsonValue::Type> QJsonRegularExpressionConverter::jsonTypes() const
{
	return {
		QJsonValue::Object,
		QJsonValue::String
	};
}

QJsonValue QJsonRegularExpressionConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(helper)

	auto regex = value.toRegularExpression();
	QJsonObject object;
	object[QStringLiteral("pattern")] = regex.pattern();
	object[QStringLiteral("options")] = static_cast<int>(regex.patternOptions());
	return object;
}

QVariant QJsonRegularExpressionConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)
	Q_UNUSED(helper)

	QString pattern;
	QRegularExpression::PatternOptions options;
	if(value.type() == QJsonValue::Object) {
		auto object = value.toObject();
		if(object.keys().size() != 2 ||
		   !object.keys().contains(QStringLiteral("pattern")) ||
		   !object.keys().contains(QStringLiteral("options")))
			throw QJsonDeserializationException("Json object has no pattern or options properties or does have extra properties");

		pattern = object.value(QStringLiteral("pattern")).toString();
		options = static_cast<QRegularExpression::PatternOptions>(object.value(QStringLiteral("options")).toInt());
	} else if(value.type() == QJsonValue::String)
		pattern = value.toString();
	else
		throw QJsonDeserializationException("Invalid json value. Must be an object or a string");

	if(pattern.isEmpty())
		return QRegularExpression();
	QRegularExpression regex(pattern, options);
	if(!regex.isValid())
		throw QJsonDeserializationException("Invalid regular expression pattern");
	return regex;
}
