#ifndef QTJSONSERIALIZER_CBORSERIALIZER_P_H
#define QTJSONSERIALIZER_CBORSERIALIZER_P_H

#include "cborserializer.h"
#include "serializerbase_p.h"

#include <optional>

#include <QtCore/QHash>
#include <QtCore/QLoggingCategory>

namespace QtJsonSerializer {

class Q_JSONSERIALIZER_EXPORT CborSerializerPrivate : public SerializerBasePrivate
{
	Q_DECLARE_PUBLIC(CborSerializer)

public:
	using ExtendedTags = CborSerializer::ExtendedTags;
	using CustomTags = CborSerializer::CustomTags;

	mutable QReadWriteLock typeTagsLock {};
	QHash<int, QCborTag> typeTags {};
	bool handleSpecialNumbers = false;

	QVariant deserializeCborValue(int propertyType, const QCborValue &value) const override;

	QVariant deserializePositiveBignum(const QByteArray &data) const;
	QVariant deserializeNegativeBignum(const QByteArray &data) const;
	qreal deserializeDecimal(const QCborArray &data) const;
	qreal deserializeBigfloat(const QCborArray &data) const;
	qreal deserializeRationaleNumber(const QCborArray &data) const;
};

Q_DECLARE_LOGGING_CATEGORY(logCbor)

}

#endif // QTJSONSERIALIZER_CBORSERIALIZER_P_H
