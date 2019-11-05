#ifndef QCBORSERIALIZER_P_H
#define QCBORSERIALIZER_P_H

#include "qcborserializer.h"
#include "qjsonserializerbase_p.h"

#include <QtCore/QHash>

namespace QtJsonSerializer {

class QCborSerializerPrivate : public QJsonSerializerBasePrivate
{
	Q_DECLARE_PUBLIC(QCborSerializer)

public:
	mutable QReadWriteLock typeTagsLock {};
	QHash<int, QCborTag> typeTags {};
};

}

#endif // QCBORSERIALIZER_P_H
