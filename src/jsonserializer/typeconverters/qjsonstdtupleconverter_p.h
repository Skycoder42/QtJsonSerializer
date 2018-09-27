#ifndef QJSONSTDTUPLECONVERTER_P_H
#define QJSONSTDTUPLECONVERTER_P_H

#include <tuple>

#include <QtCore/QRegularExpression>

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

class Q_JSONSERIALIZER_EXPORT QJsonStdTupleConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;

private:
	static const QRegularExpression tupleTypeRegex;

	QList<int> getSubtypes(int metaType) const;
};

#endif // QJSONSTDTUPLECONVERTER_P_H
