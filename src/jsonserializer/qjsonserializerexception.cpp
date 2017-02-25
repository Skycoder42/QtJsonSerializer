#include "qjsonserializerexception.h"

QJsonSerializerException::QJsonSerializerException(const QByteArray &what, bool deser) :
	QException(),
	_what(what),
	_isDeser(deser)
{}

bool QJsonSerializerException::isDeserializationException() const
{
	return _isDeser;
}

const char *QJsonSerializerException::what() const noexcept
{
	return _what.constData();
}

void QJsonSerializerException::raise() const
{
	throw *this;
}

QException *QJsonSerializerException::clone() const
{
	return new QJsonSerializerException(_what, _isDeser);
}

QJsonSerializationException::QJsonSerializationException(const QByteArray &what) :
	QJsonSerializerException("Failed to serialize with error: " + what, false)
{}

void QJsonSerializationException::raise() const
{
	throw *this;
}

QException *QJsonSerializationException::clone() const
{
	auto exc = new QJsonSerializationException("");
	exc->_what = _what;
	return exc;
}

QJsonDeserializationException::QJsonDeserializationException(const QByteArray &what) :
	QJsonSerializerException("Failed to deserialize with error: " + what, true)
{}

void QJsonDeserializationException::raise() const
{
	throw *this;
}

QException *QJsonDeserializationException::clone() const
{
	auto exc = new QJsonDeserializationException("");
	exc->_what = _what;
	return exc;
}
