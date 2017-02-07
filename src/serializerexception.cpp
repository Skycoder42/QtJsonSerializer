#include "serializerexception.h"

SerializerException::SerializerException(const QString &what, bool deser) :
	std::exception(),
	_what(QStringLiteral("Failed to %1 with error: %2")
		  .arg(deser ? "deserialize" : "serialize")
		  .arg(what)
		  .toUtf8()),
	_isDeser(deser)
{}

bool SerializerException::isDeserializationException() const
{
	return _isDeser;
}

QString SerializerException::qWhat() const
{
	return QString::fromUtf8(_what);
}

const char *SerializerException::what() const noexcept
{
	return _what.constData();
}

SerializationException::SerializationException(const QString &what) :
	SerializerException(what, false)
{}

DeserializationException::DeserializationException(const QString &what) :
	SerializerException(what, true)
{}
