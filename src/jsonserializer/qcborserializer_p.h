#ifndef QCBORSERIALIZER_P_H
#define QCBORSERIALIZER_P_H

#include "qcborserializer.h"
#include "qjsonserializer_p.h"

#include <QtCore/QHash>

class QCborSerializerPrivate : public QJsonSerializerPrivate
{
public:
	using ByteArrayTag = QCborSerializer::ByteArrayTag;

	QReadWriteLock typeTagsLock {};
	QHash<int, QCborTag> typeTags {};

	ByteArrayTag byteArrayTag = ByteArrayTag::None;

	QSharedPointer<QCborTypeConverter> findConverter(int propertyType,
													 QCborValue::Type valueType = QCborValue::Undefined,
													 QCborTag tag = QCborTypeConverter::NoTag);
};

#endif // QCBORSERIALIZER_P_H
