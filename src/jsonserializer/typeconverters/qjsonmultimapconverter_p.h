#ifndef QJSONMULTIMAPCONVERTER_P_H
#define QJSONMULTIMAPCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

#include <QtCore/QRegularExpression>

class Q_JSONSERIALIZER_EXPORT QJsonMultiMapConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> allowedCborTags(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;

private:
	static const QRegularExpression mapTypeRegex;

	std::pair<int, int> getSubtype(int mapType) const;
};

#endif // QJSONMULTIMAPCONVERTER_P_H
