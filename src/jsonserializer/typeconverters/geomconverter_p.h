#ifndef QTJSONSERIALIZER_GEOMCONVERTER_P_H
#define QTJSONSERIALIZER_GEOMCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "typeconverter.h"

#include <QtCore/QCborArray>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtCore/QLine>
#include <QtCore/QRect>

namespace QtJsonSerializer::TypeConverters {

class Q_JSONSERIALIZER_EXPORT GeomConverter : public TypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> allowedCborTags(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	int guessType(QCborTag tag, QCborValue::Type dataType) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;

private:
	QCborValue serializeSize(const std::variant<QSize, QSizeF> &size) const;
	QCborValue serializePoint(const std::variant<QPoint, QPointF> &point) const;
	QCborValue serializeLine(const std::variant<QLine, QLineF> &line) const;
	QCborValue serializeRect(const std::variant<QRect, QRectF> &rect) const;

	template <typename TSize>
	TSize deserializeSize(const QCborArray &array) const;
	template <typename TPoint>
	TPoint deserializePoint(const QCborArray &array) const;
	template <typename TLine>
	TLine deserializeLine(const QCborArray &array) const;
	template <typename TRect>
	TRect deserializeRect(const QCborArray &array) const;
	template <typename TValue>
	TValue extract(const QCborValue &value) const;
};

template <>
int GeomConverter::extract(const QCborValue &value) const;

template <>
qreal GeomConverter::extract(const QCborValue &value) const;

}

#endif // QTJSONSERIALIZER_GEOMCONVERTER_P_H
