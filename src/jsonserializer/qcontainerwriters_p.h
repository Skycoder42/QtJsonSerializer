#ifndef QCONTAINERWRITERS_P_H
#define QCONTAINERWRITERS_P_H

#include "qcontainerwriters.h"

class Q_JSONSERIALIZER_EXPORT QContainerWritersPrivate
{
public:
	static QReadWriteLock sequenceLock;
	static QHash<int, QSequentialWriterFactory*> sequenceFactories;
};

#endif // QCONTAINERWRITERS_P_H
