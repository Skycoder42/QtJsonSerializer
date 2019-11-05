#ifndef QTJSONSERIALIZER_JSONSERIALIZER_P_H
#define QTJSONSERIALIZER_JSONSERIALIZER_P_H

#include "jsonserializer.h"
#include "serializerbase_p.h"

namespace QtJsonSerializer {

class Q_JSONSERIALIZER_EXPORT JsonSerializerPrivate : public SerializerBasePrivate
{
	Q_DECLARE_PUBLIC(JsonSerializer)

public:
	using ByteArrayFormat = JsonSerializer::ByteArrayFormat;
	ByteArrayFormat byteArrayFormat = ByteArrayFormat::Base64;
	bool validateBase64 = true;
};

}

#endif // QTJSONSERIALIZER_JSONSERIALIZER_P_H
