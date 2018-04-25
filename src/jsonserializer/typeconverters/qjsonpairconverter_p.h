#ifndef QJSONPAIRCONVERTER_P_H
#define QJSONPAIRCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

#include <QtCore/QRegularExpression>

class Q_JSONSERIALIZER_EXPORT QJsonPairConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;

private:
	static const QRegularExpression pairTypeRegex;

	static QPair<int, int> getPairTypes(int metaType);
};

#endif // QJSONPAIRCONVERTER_P_H
