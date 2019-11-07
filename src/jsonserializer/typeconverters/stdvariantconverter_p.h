#ifndef QTJSONSERIALIZER_STDVARIANTCONVERTER_P_H
#define QTJSONSERIALIZER_STDVARIANTCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "typeconverter.h"

namespace QtJsonSerializer::TypeConverters {

class Q_JSONSERIALIZER_EXPORT StdVariantConverter : public TypeConverter
{
public:
	QT_JSONSERIALIZER_TYPECONVERTER_NAME(StdVariantConverter)
	bool canConvert(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;
};

}

#endif // QTJSONSERIALIZER_STDVARIANTCONVERTER_P_H
