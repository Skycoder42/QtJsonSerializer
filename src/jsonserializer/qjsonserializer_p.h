#ifndef QJSONSERIALIZER_P_H
#define QJSONSERIALIZER_P_H

#include "qjsonserializer.h"
#include "qjsonserializerbase_p.h"

class Q_JSONSERIALIZER_EXPORT QJsonSerializerPrivate : public QJsonSerializerBasePrivate
{
	Q_DECLARE_PUBLIC(QJsonSerializer)

public:
	using ByteArrayFormat = QJsonSerializer::ByteArrayFormat;
	ByteArrayFormat byteArrayFormat = ByteArrayFormat::Base64;
	bool validateBase64 = true;
};

#endif // QJSONSERIALIZER_P_H
