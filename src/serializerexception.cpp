#include "serializerexception.h"

SerializerException::SerializerException(const QString &what, bool deser) :
	QException(),
	_what(QStringLiteral("Failed to %1 with error: %2")
		  .arg(deser ? "deserialize" : "serialize")
		  .arg(what)
		  .toUtf8()),
	_isDeser(deser)
{}

SerializerException::SerializerException(const QByteArray &what, bool deser) :
	_what(what),
	_isDeser(deser)
{}

bool SerializerException::isDeserializationException() const
{
	return _isDeser;
}

const char *SerializerException::what() const noexcept
{
	return _what.constData();
}

void SerializerException::raise() const
{
	throw *this;
}

QException *SerializerException::clone() const
{
	return new SerializerException(_what, _isDeser);
}

SerializationException::SerializationException(const QString &what) :
	SerializerException(what, false)
{}

void SerializationException::raise() const
{
	throw *this;
}

QException *SerializationException::clone() const
{
	return new SerializationException(_what);
}

DeserializationException::DeserializationException(const QString &what) :
	SerializerException(what, true)
{}

void DeserializationException::raise() const
{
	throw *this;
}

QException *DeserializationException::clone() const
{
	return new DeserializationException(_what);
}
