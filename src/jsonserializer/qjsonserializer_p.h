#ifndef QJSONSERIALIZER_P_H
#define QJSONSERIALIZER_P_H

#include "qjsonserializer_global.h"
#include "qjsonserializer.h"

class Q_JSONSERIALIZER_EXPORT QJsonSerializerPrivate
{
public:
	bool allowNull;
	bool keepObjectName;
	bool enumAsString;

	QJsonSerializerPrivate();
};

#endif // QJSONSERIALIZER_P_H
