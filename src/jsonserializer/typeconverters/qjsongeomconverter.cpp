#include "qjsongeomconverter_p.h"
#include "qjsonserializerexception.h"

#include <QtCore/QJsonObject>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtCore/QLine>
#include <QtCore/QRect>

bool QJsonSizeConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QSize ||
			metaTypeId == QMetaType::QSizeF;
}

QList<QJsonValue::Type> QJsonSizeConverter::jsonTypes() const
{
	return {QJsonValue::Object};
}

QJsonValue QJsonSizeConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper)

	QJsonValue w;
	QJsonValue h;
	if(propertyType == QMetaType::QSize) {
		auto size = value.toSize();
		w = size.width();
		h = size.height();
	} else if(propertyType == QMetaType::QSizeF) {
		auto size = value.toSizeF();
		w = size.width();
		h = size.height();
	} else
		throw QJsonSerializationException(QByteArray("Invalid metatype: ") + QMetaType::typeName(propertyType));

	QJsonObject object;
	object[QStringLiteral("width")] = w;
	object[QStringLiteral("height")] = h;
	return object;
}

QVariant QJsonSizeConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(parent)
	Q_UNUSED(helper)

	auto object = value.toObject();
	if(object.keys().size() != 2 ||
	   !object.keys().contains(QStringLiteral("width")) ||
	   !object.keys().contains(QStringLiteral("height")))
		throw QJsonDeserializationException("Json object has no width or height properties or does have extra properties");

	auto w = object.value(QStringLiteral("width"));
	auto h = object.value(QStringLiteral("height"));
	if(!w.isDouble() || !h.isDouble())
		throw QJsonDeserializationException("Object properties width and height must be numbers");

	if(propertyType == QMetaType::QSize)
		return QSize(w.toInt(), h.toInt());
	else if(propertyType == QMetaType::QSizeF)
		return QSizeF(w.toDouble(), h.toDouble());
	else
		throw QJsonDeserializationException(QByteArray("Invalid metatype: ") + QMetaType::typeName(propertyType));
}



bool QJsonPointConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QPoint ||
			metaTypeId == QMetaType::QPointF;
}

QList<QJsonValue::Type> QJsonPointConverter::jsonTypes() const
{
	return {QJsonValue::Object};
}

QJsonValue QJsonPointConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper)

	QJsonValue x;
	QJsonValue y;
	if(propertyType == QMetaType::QPoint) {
		auto point = value.toPoint();
		x = point.x();
		y = point.y();
	} else if(propertyType == QMetaType::QPointF) {
		auto point = value.toPointF();
		x = point.x();
		y = point.y();
	} else
		throw QJsonSerializationException(QByteArray("Invalid metatype: ") + QMetaType::typeName(propertyType));

	QJsonObject object;
	object[QStringLiteral("x")] = x;
	object[QStringLiteral("y")] = y;
	return object;
}

QVariant QJsonPointConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(parent)
	Q_UNUSED(helper)

	auto object = value.toObject();
	if(object.keys().size() != 2 ||
	   !object.keys().contains(QStringLiteral("x")) ||
	   !object.keys().contains(QStringLiteral("y")))
		throw QJsonDeserializationException("Json object has no x or y properties or does have extra properties");

	auto x = object.value(QStringLiteral("x"));
	auto y = object.value(QStringLiteral("y"));
	if(!x.isDouble() || !y.isDouble())
		throw QJsonDeserializationException("Object properties x and y must be numbers");

	if(propertyType == QMetaType::QPoint)
		return QPoint(x.toInt(), y.toInt());
	else if(propertyType == QMetaType::QPointF)
		return QPointF(x.toDouble(), y.toDouble());
	else
		throw QJsonDeserializationException(QByteArray("Invalid metatype: ") + QMetaType::typeName(propertyType));
}

bool QJsonLineConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QLine ||
			metaTypeId == QMetaType::QLineF;
}

QList<QJsonValue::Type> QJsonLineConverter::jsonTypes() const
{
	return {QJsonValue::Object};
}

QJsonValue QJsonLineConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	QJsonValue p1;
	QJsonValue p2;
	if(propertyType == QMetaType::QLine) {
		auto line = value.toLine();
		p1 = helper->serializeSubtype(QMetaType::QPoint, line.p1(), "p1");
		p2 = helper->serializeSubtype(QMetaType::QPoint, line.p2(), "p2");
	} else if(propertyType == QMetaType::QLineF) {
		auto line = value.toLineF();
		p1 = helper->serializeSubtype(QMetaType::QPointF, line.p1(), "p1");
		p2 = helper->serializeSubtype(QMetaType::QPointF, line.p2(), "p2");
	} else
		throw QJsonSerializationException(QByteArray("Invalid metatype: ") + QMetaType::typeName(propertyType));

	QJsonObject object;
	object[QStringLiteral("p1")] = p1;
	object[QStringLiteral("p2")] = p2;
	return object;
}

QVariant QJsonLineConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto object = value.toObject();
	if(object.keys().size() != 2 ||
	   !object.keys().contains(QStringLiteral("p1")) ||
	   !object.keys().contains(QStringLiteral("p2")))
		throw QJsonDeserializationException("Json object has no p1 or p2 properties or does have extra properties");

	auto v1 = object.value(QStringLiteral("p1"));
	auto v2 = object.value(QStringLiteral("p2"));
	if(propertyType == QMetaType::QLine) {
		auto p1 = helper->deserializeSubtype(QMetaType::QPoint, v1, parent, "p1");
		auto p2 = helper->deserializeSubtype(QMetaType::QPoint, v2, parent, "p1");
		return QLine(p1.toPoint(), p2.toPoint());
	} else if(propertyType == QMetaType::QLineF) {
		auto p1 = helper->deserializeSubtype(QMetaType::QPointF, v1, parent, "p1");
		auto p2 = helper->deserializeSubtype(QMetaType::QPointF, v2, parent, "p1");
		return QLineF(p1.toPointF(), p2.toPointF());
	} else
		throw QJsonDeserializationException(QByteArray("Invalid metatype: ") + QMetaType::typeName(propertyType));
}

bool QJsonRectConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QRect ||
			metaTypeId == QMetaType::QRectF;
}

QList<QJsonValue::Type> QJsonRectConverter::jsonTypes() const
{
	return {QJsonValue::Object};
}

QJsonValue QJsonRectConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	QJsonValue p1;
	QJsonValue p2;
	if(propertyType == QMetaType::QRect) {
		auto rect = value.toRect();
		p1 = helper->serializeSubtype(QMetaType::QPoint, rect.topLeft(), "topLeft");
		p2 = helper->serializeSubtype(QMetaType::QPoint, rect.bottomRight(), "bottomRight");
	} else if(propertyType == QMetaType::QRectF) {
		auto rect = value.toRectF();
		p1 = helper->serializeSubtype(QMetaType::QPointF, rect.topLeft(), "topLeft");
		p2 = helper->serializeSubtype(QMetaType::QPointF, rect.bottomRight(), "bottomRight");
	} else
		throw QJsonSerializationException(QByteArray("Invalid metatype: ") + QMetaType::typeName(propertyType));

	QJsonObject object;
	object[QStringLiteral("topLeft")] = p1;
	object[QStringLiteral("bottomRight")] = p2;
	return object;
}

QVariant QJsonRectConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto object = value.toObject();
	if(object.keys().size() != 2 ||
	   !object.keys().contains(QStringLiteral("topLeft")) ||
	   !object.keys().contains(QStringLiteral("bottomRight")))
		throw QJsonDeserializationException("Json object has no topLeft or bottomRight properties or does have extra properties");

	auto v1 = object.value(QStringLiteral("topLeft"));
	auto v2 = object.value(QStringLiteral("bottomRight"));
	if(propertyType == QMetaType::QRect) {
		auto topLeft = helper->deserializeSubtype(QMetaType::QPoint, v1, parent, "topLeft");
		auto bottomRight = helper->deserializeSubtype(QMetaType::QPoint, v2, parent, "bottomRight");
		return QRect(topLeft.toPoint(), bottomRight.toPoint());
	} else if(propertyType == QMetaType::QRectF) {
		auto topLeft = helper->deserializeSubtype(QMetaType::QPointF, v1, parent, "topLeft");
		auto bottomRight = helper->deserializeSubtype(QMetaType::QPointF, v2, parent, "bottomRight");
		return QRectF(topLeft.toPointF(), bottomRight.toPointF());
	} else
		throw QJsonDeserializationException(QByteArray("Invalid metatype: ") + QMetaType::typeName(propertyType));
}
