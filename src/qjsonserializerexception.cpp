#include "qjsonserializerexception.h"

QJsonSerializerException::QJsonSerializerException(const QString &what, bool deser) :
	QException(),
	_what(QStringLiteral("Failed to %1 with error: %2")
		  .arg(deser ? "deserialize" : "serialize")
		  .arg(what)
		  .toUtf8()),
	_isDeser(deser)
{}

QJsonSerializerException::QJsonSerializerException(const QByteArray &what, bool deser) :
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

QJsonSerializationException::QJsonSerializationException(const QString &what) :
	QJsonSerializerException(what, false)
{}

void QJsonSerializationException::raise() const
{
	throw *this;
}

QException *QJsonSerializationException::clone() const
{
	return new QJsonSerializationException(_what);
}

QJsonDeserializationException::QJsonDeserializationException(const QString &what) :
	QJsonSerializerException(what, true)
{}

void QJsonDeserializationException::raise() const
{
	throw *this;
}

QException *QJsonDeserializationException::clone() const
{
	return new QJsonDeserializationException(_what);
}
