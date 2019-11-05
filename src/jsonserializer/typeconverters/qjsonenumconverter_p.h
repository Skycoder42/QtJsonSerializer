#ifndef QJSONENUMCONVERTER_H
#define QJSONENUMCONVERTER_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

#include <QtCore/QMetaEnum>

namespace QtJsonSerializer::TypeConverters {

class Q_JSONSERIALIZER_EXPORT QJsonEnumConverter : public QJsonTypeConverter
{
public:
	QJsonEnumConverter();
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

#endif // QJSONENUMCONVERTER_H
