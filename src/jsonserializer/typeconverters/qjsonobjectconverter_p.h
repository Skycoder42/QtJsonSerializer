#ifndef QJSONOBJECTCONVERTER_P_H
#define QJSONOBJECTCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

class Q_JSONSERIALIZER_EXPORT QJsonObjectConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;

private:
	template<typename T>
	static T extract(QVariant variant) ;
	static QVariant toVariant(QObject *object, QMetaType::TypeFlags flags);
};

#endif // QJSONOBJECTCONVERTER_P_H
