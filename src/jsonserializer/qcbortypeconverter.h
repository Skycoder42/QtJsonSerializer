#ifndef QCBORTYPECONVERTER_H
#define QCBORTYPECONVERTER_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include "QtJsonSerializer/qjsontypeconverter.h"

#include <QtCore/qcborvalue.h>

class Q_JSONSERIALIZER_EXPORT QCborTypeConverter : public QJsonTypeConverter
{
public:
	static constexpr QCborTag NoTag = static_cast<QCborTag>(-1);

	virtual QList<QCborValue::Type> cborTypes() const = 0;
	virtual QList<QCborTag> allowedCborTags() const;
	virtual QCborValue serializeCbor(int propertyType, const QVariant &value, const SerializationHelper *helper) const = 0;
	virtual QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const SerializationHelper *helper) const = 0;

	// QJsonTypeConverter interface
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;

protected:
	virtual QList<QJsonValue::Type> cborToJsonType(QCborValue::Type type) const;
};

#endif // QCBORTYPECONVERTER_H
