#ifndef QJSONSTDTUPLECONVERTER_P_H
#define QJSONSTDTUPLECONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

namespace QtJsonSerializer::TypeConverters {

class Q_JSONSERIALIZER_EXPORT QJsonStdTupleConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> allowedCborTags(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;
};

}

#endif // QJSONSTDTUPLECONVERTER_P_H
