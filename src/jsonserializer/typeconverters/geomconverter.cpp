#include "geomconverter_p.h"
#include "exception.h"
#include "cborserializer.h"
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool GeomConverter::canConvert(int metaTypeId) const
{
	static const QVector<int> types {
		QMetaType::QSize,
		QMetaType::QSizeF,
		QMetaType::QPoint,
		QMetaType::QPointF,
		QMetaType::QLine,
		QMetaType::QLineF,
		QMetaType::QRect,
		QMetaType::QRectF,
	};
	return types.contains(metaTypeId);
}

QList<QCborTag> GeomConverter::allowedCborTags(int metaTypeId) const
{
	switch (metaTypeId) {
	case QMetaType::QSize:
	case QMetaType::QSizeF:
		return {static_cast<QCborTag>(CborSerializer::GeomSize)};
	case QMetaType::QPoint:
	case QMetaType::QPointF:
		return {static_cast<QCborTag>(CborSerializer::GeomPoint)};
	case QMetaType::QLine:
	case QMetaType::QLineF:
		return {static_cast<QCborTag>(CborSerializer::GeomLine)};
	case QMetaType::QRect:
	case QMetaType::QRectF:
		return {static_cast<QCborTag>(CborSerializer::GeomRect)};
	default:
		return {};
	}
}

QList<QCborValue::Type> GeomConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array};
}

int GeomConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	if (dataType != QCborValue::Array)
		return QMetaType::UnknownType;
	switch (tag) {
	case static_cast<QCborTag>(CborSerializer::GeomSize):
		return QMetaType::QSizeF;
	case static_cast<QCborTag>(CborSerializer::GeomPoint):
		return QMetaType::QPointF;
	case static_cast<QCborTag>(CborSerializer::GeomLine):
		return QMetaType::QLineF;
	case static_cast<QCborTag>(CborSerializer::GeomRect):
		return QMetaType::QRectF;
	default:
		return QMetaType::UnknownType;
	}
}

QCborValue GeomConverter::serialize(int propertyType, const QVariant &value) const
{
	switch (propertyType) {
	case QMetaType::QSize:
		return serializeSize(value.toSize());
	case QMetaType::QSizeF:
		return serializeSize(value.toSizeF());
	case QMetaType::QPoint:
		return serializePoint(value.toPoint());
	case QMetaType::QPointF:
		return serializePoint(value.toPointF());
	case QMetaType::QLine:
		return serializeLine(value.toLine());
	case QMetaType::QLineF:
		return serializeLine(value.toLineF());
	case QMetaType::QRect:
		return serializeRect(value.toRect());
	case QMetaType::QRectF:
		return serializeRect(value.toRectF());
	default:
		throw SerializationException{"Invalid type id"};
	}
}

QVariant GeomConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(parent)
	const auto array = (value.isTag() ? value.taggedValue() : value).toArray();
	switch (propertyType) {
	case QMetaType::QSize:
		return deserializeSize<QSize>(array);
	case QMetaType::QSizeF:
		return deserializeSize<QSizeF>(array);
	case QMetaType::QPoint:
		return deserializePoint<QPoint>(array);
	case QMetaType::QPointF:
		return deserializePoint<QPointF>(array);
	case QMetaType::QLine:
		return deserializeLine<QLine>(array);
	case QMetaType::QLineF:
		return deserializeLine<QLineF>(array);
	case QMetaType::QRect:
		return deserializeRect<QRect>(array);
	case QMetaType::QRectF:
		return deserializeRect<QRectF>(array);
	default:
		throw DeserializationException{"Invalid type id"};
	}
}

QCborValue GeomConverter::serializeSize(const std::variant<QSize, QSizeF> &size) const
{
	return {
		static_cast<QCborTag>(CborSerializer::GeomSize),
		std::visit([](const auto &s) -> QCborArray {
			return {s.width(), s.height()};
		}, size)
	};
}

QCborValue GeomConverter::serializePoint(const std::variant<QPoint, QPointF> &point) const
{
	return {
		static_cast<QCborTag>(CborSerializer::GeomPoint),
		std::visit([](const auto &p) -> QCborArray {
			return {p.x(), p.y()};
		}, point)
	};
}

QCborValue GeomConverter::serializeLine(const std::variant<QLine, QLineF> &line) const
{
	return {
		static_cast<QCborTag>(CborSerializer::GeomLine),
		std::visit([this](const auto &l) -> QCborArray {
			return {
				helper()->serializeSubtype(qMetaTypeId<std::decay_t<decltype(l.p1())>>(), l.p1(), "p1"),
				helper()->serializeSubtype(qMetaTypeId<std::decay_t<decltype(l.p2())>>(), l.p2(), "p2")
			};
		}, line)
	};
}

QCborValue GeomConverter::serializeRect(const std::variant<QRect, QRectF> &rect) const
{
	return {
		static_cast<QCborTag>(CborSerializer::GeomRect),
		std::visit([this](const auto &r) -> QCborArray {
			return {
				helper()->serializeSubtype(qMetaTypeId<std::decay_t<decltype(r.topLeft())>>(), r.topLeft(), "topLeft"),
				helper()->serializeSubtype(qMetaTypeId<std::decay_t<decltype(r.size())>>(), r.size(), "size")
			};
		}, rect)
	};
}

template<typename TSize>
TSize GeomConverter::deserializeSize(const QCborArray &array) const
{
	using TW = std::decay_t<decltype(TSize{}.width())>;
	using TH = std::decay_t<decltype(TSize{}.height())>;
	if (array.size() != 2)
		throw DeserializationException{"A size requires an array with exactly two numbers"};
	return {
		extract<TW>(array[0]),
		extract<TH>(array[1])
	};
}

template<typename TPoint>
TPoint GeomConverter::deserializePoint(const QCborArray &array) const
{
	using TX = std::decay_t<decltype(TPoint{}.x())>;
	using TY = std::decay_t<decltype(TPoint{}.y())>;
	if (array.size() != 2)
		throw DeserializationException{"A point requires an array with exactly two numbers"};
	return {
		extract<TX>(array[0]),
		extract<TY>(array[1])
	};
}

template<typename TLine>
TLine GeomConverter::deserializeLine(const QCborArray &array) const
{
	using TP1 = std::decay_t<decltype(TLine{}.p1())>;
	using TP2 = std::decay_t<decltype(TLine{}.p2())>;
	if (array.size() != 2)
		throw DeserializationException{"A line requires an array with exactly two points"};
	return {
		helper()->deserializeSubtype(qMetaTypeId<TP1>(), array[0], nullptr, "p1").template value<TP1>(),
		helper()->deserializeSubtype(qMetaTypeId<TP2>(), array[1], nullptr, "p2").template value<TP2>()
	};
}

template<typename TRect>
TRect GeomConverter::deserializeRect(const QCborArray &array) const
{
	using TTL = std::decay_t<decltype(TRect{}.topLeft())>;
	using TS = std::decay_t<decltype(TRect{}.size())>;
	if (array.size() != 2)
		throw DeserializationException{"A line requires an array with exactly two points"};
	return {
		helper()->deserializeSubtype(qMetaTypeId<TTL>(), array[0], nullptr, "topLeft").template value<TTL>(),
		helper()->deserializeSubtype(qMetaTypeId<TS>(), array[1], nullptr, "size").template value<TS>()
	};
}

template<>
int GeomConverter::extract(const QCborValue &value) const
{
	if (!value.isInteger())
		throw DeserializationException{"Expected integer, but got type " + QByteArray::number(value.type())};
	return static_cast<int>(value.toInteger());
}

template<>
qreal GeomConverter::extract(const QCborValue &value) const
{
	if (!value.isDouble() && !value.isInteger())
		throw DeserializationException{"Expected double, but got type " + QByteArray::number(value.type())};
	return value.toDouble();
}
