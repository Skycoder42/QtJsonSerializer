#ifndef QJSONENUMCONVERTER_P_H
#define QJSONENUMCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

#include <QtCore/QMetaProperty>

class Q_JSONSERIALIZER_EXPORT QJsonEnumConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;

private:
	template <typename T>
	static QMetaEnum getMetaEnum(int typeId);
};

#endif // QJSONENUMCONVERTER_P_H
