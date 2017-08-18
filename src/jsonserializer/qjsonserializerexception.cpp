#include "qjsonserializerexception.h"
#include "qjsonserializerexception_p.h"
#include "qjsonexceptioncontext_p.h"

QJsonSerializerException::QJsonSerializerException(const QByteArray &what) :
	QException(),
	d(new QJsonSerializationExceptionPrivate(what))
{}

const char *QJsonSerializerException::what() const noexcept
{
	return d->what.constData();
}

QByteArray QJsonSerializerException::message() const
{
	return d->what;
}

QJsonSerializerException::PropertyTrace QJsonSerializerException::propertyTrace() const
{
	return d->trace;
}

void QJsonSerializerException::raise() const
{
	throw *this;
}

QException *QJsonSerializerException::clone() const
{
	auto exc = new QJsonSerializerException(QByteArray());
	exc->d = d;
	return exc;
}

QJsonSerializationException::QJsonSerializationException(const QByteArray &what) :
	QJsonSerializerException("Failed to serialize with error: " + what)
{}

void QJsonSerializationException::raise() const
{
	throw *this;
}

QException *QJsonSerializationException::clone() const
{
	auto exc = new QJsonSerializationException(QByteArray());
	exc->d = d;
	return exc;
}

QJsonDeserializationException::QJsonDeserializationException(const QByteArray &what) :
	QJsonSerializerException("Failed to deserialize with error: " + what)
{}

void QJsonDeserializationException::raise() const
{
	throw *this;
}

QException *QJsonDeserializationException::clone() const
{
	auto exc = new QJsonDeserializationException(QByteArray());
	exc->d = d;
	return exc;
}



QJsonSerializationExceptionPrivate::QJsonSerializationExceptionPrivate(QByteArray message) :
	message(message),
	trace(QJsonExceptionContext::currentContext()),
	what()
{
	//construct the whole trace
	what = "what: " + message + "\nProperty Trace:";
	if(trace.isEmpty())
		what += " <root element>";
	else {
		foreach(auto p, trace)
			what += "\n\t" + p.first + " (Type: " + p.second + ")";
	}
}
