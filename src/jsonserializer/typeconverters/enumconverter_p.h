#ifndef QTJSONSERIALIZER_ENUMCONVERTER_H
#define QTJSONSERIALIZER_ENUMCONVERTER_H

#include "qtjsonserializer_global.h"
#include "typeconverter.h"

#include <QtCore/QMetaEnum>

namespace QtJsonSerializer::TypeConverters {

class Q_JSONSERIALIZER_EXPORT EnumConverter : public TypeConverter
{
public:
	EnumConverter();
	QT_JSONSERIALIZER_TYPECONVERTER_NAME(EnumConverter)
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> allowedCborTags(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;
	QVariant deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const override;

private:
	bool testForEnum(int metaTypeId) const;
	QMetaEnum getEnum(int metaTypeId, bool ser) const;
};

}

#endif // QTJSONSERIALIZER_ENUMCONVERTER_H
