#ifndef QJSONCHRONODURATIONCONVERTER_P_H
#define QJSONCHRONODURATIONCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qcbortypeconverter.h"

class Q_JSONSERIALIZER_EXPORT QJsonChronoDurationConverter : public QCborTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> cborTags() const override;
	QList<QCborValue::Type> cborTypes() const override;
	QCborValue serializeCbor(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const SerializationHelper *helper) const override;

	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const override;

private:
	QCborTag tagForType(int metaTypeId) const;
};

#endif // QJSONCHRONODURATIONCONVERTER_P_H
