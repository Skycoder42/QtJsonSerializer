#ifndef QJSONLISTCONVERTER_P_H
#define QJSONLISTCONVERTER_P_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include "QtJsonSerializer/qjsontypeconverter.h"

#include <QtCore/QRegularExpression>

class Q_JSONSERIALIZER_EXPORT QJsonListConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> allowedCborTags(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;

private:
	static const QRegularExpression listTypeRegex;

	int getSubtype(int listType) const;
};

#endif // QJSONLISTCONVERTER_P_H
