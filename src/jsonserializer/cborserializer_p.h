#ifndef QTJSONSERIALIZER_CBORSERIALIZER_P_H
#define QTJSONSERIALIZER_CBORSERIALIZER_P_H

#include "cborserializer.h"
#include "serializerbase_p.h"

#include <QtCore/QHash>
#include <QtCore/QLoggingCategory>

namespace QtJsonSerializer {

class Q_JSONSERIALIZER_EXPORT CborSerializerPrivate : public SerializerBasePrivate
{
	Q_DECLARE_PUBLIC(CborSerializer)

public:
	mutable QReadWriteLock typeTagsLock {};
	QHash<int, QCborTag> typeTags {};
};

Q_DECLARE_LOGGING_CATEGORY(logCbor)

}

#endif // QTJSONSERIALIZER_CBORSERIALIZER_P_H
