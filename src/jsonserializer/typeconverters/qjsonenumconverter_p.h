#ifndef QJSONENUMCONVERTER_H
#define QJSONENUMCONVERTER_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

#include <QtCore/QMetaEnum>

class Q_JSONSERIALIZER_EXPORT QJsonEnumConverter : public QJsonTypeConverter
{
public:
	static QJsonValue serializeEnum(const QMetaEnum &metaEnum, const QVariant &value, bool enumAsString);
	static QVariant deserializeEnum(const QMetaEnum &metaEnum, const QJsonValue &value);

	QJsonEnumConverter();

	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;

private:
	bool testForEnum(int metaTypeId) const;
	QMetaEnum getEnum(int metaTypeId, bool ser) const;
};

#endif // QJSONENUMCONVERTER_H
