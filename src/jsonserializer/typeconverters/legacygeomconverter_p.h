#ifndef QTJSONSERIALIZER_LEGACYGEOMCONVERTER_P_H
#define QTJSONSERIALIZER_LEGACYGEOMCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "typeconverter.h"

#include <QtCore/QCborArray>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtCore/QLine>
#include <QtCore/QRect>

namespace QtJsonSerializer::TypeConverters {

class Q_JSONSERIALIZER_EXPORT LegacyGeomConverter : public TypeConverter
{
public:
	LegacyGeomConverter();

	bool canConvert(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;
	QVariant deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const override;

private:
	template <typename TSize>
	TSize deserializeSize(const QCborMap &map) const;
	template <typename TPoint>
	TPoint deserializePoint(const QCborMap &map) const;
	template <typename TLine>
	TLine deserializeLine(const QCborMap &map) const;
	template <typename TRect>
	TRect deserializeRect(const QCborMap &map) const;
	template <typename TValue>
	TValue extract(const QCborValue &value) const;
};

template <>
int LegacyGeomConverter::extract(const QCborValue &value) const;

template <>
qreal LegacyGeomConverter::extract(const QCborValue &value) const;

}

#endif // QTJSONSERIALIZER_LEGACYGEOMCONVERTER_P_H
