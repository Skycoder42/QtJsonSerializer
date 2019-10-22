#ifndef QJSONGADGETCONVERTER_P_H
#define QJSONGADGETCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

class Q_JSONSERIALIZER_EXPORT QJsonGadgetConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;
};

#endif // QJSONGADGETCONVERTER_P_H
