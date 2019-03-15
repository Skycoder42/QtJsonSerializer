#include "qjsonserializerexception.h"
#include "qjsonserializerexception_p.h"
#include "qjsonexceptioncontext_p.h"

QJsonSerializerException::QJsonSerializerException(const QByteArray &what) :
	d{new QJsonSerializationExceptionPrivate{what}}
{}

const char *QJsonSerializerException::what() const noexcept
{
	return d->what.constData();
}

QByteArray QJsonSerializerException::message() const
{
	return d->message;
}

QJsonSerializerException::PropertyTrace QJsonSerializerException::propertyTrace() const
{
	return d->trace;
}

void QJsonSerializerException::raise() const
{
	throw *this;
}

QT_JSONSERIALIZER_EXCEPTION_BASE *QJsonSerializerException::clone() const
{
	auto exc = new QJsonSerializerException(QByteArray());
	exc->d = d;
	return exc;
}

QJsonSerializationException::QJsonSerializationException(const QByteArray &what) :
	QJsonSerializerException{"Failed to serialize with error: " + what}
{}

void QJsonSerializationException::raise() const
{
	throw *this;
}

QT_JSONSERIALIZER_EXCEPTION_BASE *QJsonSerializationException::clone() const
{
	auto exc = new QJsonSerializationException(QByteArray());
	exc->d = d;
	return exc;
}

QJsonDeserializationException::QJsonDeserializationException(const QByteArray &what) :
	QJsonSerializerException{"Failed to deserialize with error: " + what}
{}

void QJsonDeserializationException::raise() const
{
	throw *this;
}

QT_JSONSERIALIZER_EXCEPTION_BASE *QJsonDeserializationException::clone() const
{
	auto exc = new QJsonDeserializationException(QByteArray());
	exc->d = d;
	return exc;
}



QJsonSerializationExceptionPrivate::QJsonSerializationExceptionPrivate(QByteArray msg) :
	message{std::move(msg)},
	trace{QJsonExceptionContext::currentContext()}
{
	//construct the whole trace
	what = "what: " + message + "\nProperty Trace:";
	if(trace.isEmpty())
		what += " <root element>";
	else {
		for(const auto &p : qAsConst(trace))
			what += "\n\t" + p.first + " (Type: " + p.second + ")";
	}
}
