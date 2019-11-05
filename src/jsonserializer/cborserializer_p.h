#ifndef QTJSONSERIALIZER_CBORSERIALIZER_P_H
#define QTJSONSERIALIZER_CBORSERIALIZER_P_H

#include "cborserializer.h"
#include "serializerbase_p.h"

#include <QtCore/QHash>

namespace QtJsonSerializer {

class CborSerializerPrivate : public SerializerBasePrivate
{
	Q_DECLARE_PUBLIC(CborSerializer)

public:
	mutable QReadWriteLock typeTagsLock {};
	QHash<int, QCborTag> typeTags {};
};

}

#endif // QTJSONSERIALIZER_CBORSERIALIZER_P_H
