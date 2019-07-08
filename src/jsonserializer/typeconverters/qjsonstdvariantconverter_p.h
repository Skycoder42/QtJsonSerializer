#ifndef QJSONSTDVARIANTCONVERTER_P_H
#define QJSONSTDVARIANTCONVERTER_P_H

#include <QtCore/QRegularExpression>

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

class Q_JSONSERIALIZER_EXPORT QJsonStdVariantConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;

private:
	static const QRegularExpression variantTypeRegex;

	QList<int> getSubtypes(int metaType) const;
};

#endif // QJSONSTDVARIANTCONVERTER_P_H
