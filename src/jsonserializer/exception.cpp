#include "exception.h"
#include "exception_p.h"
#include "exceptioncontext_p.h"
using namespace QtJsonSerializer;

Exception::Exception(const QByteArray &what) :
	d{new ExceptionPrivate{what}}
{}

const char *Exception::what() const noexcept
{
	return d->what.constData();
}

QByteArray Exception::message() const
{
	return d->message;
}

Exception::PropertyTrace Exception::propertyTrace() const
{
	return d->trace;
}

void Exception::raise() const
{
	throw *this;
}

ExceptionBase *Exception::clone() const
{
	auto exc = new Exception(QByteArray());
	exc->d = d;
	return exc;
}

SerializationException::SerializationException(const QByteArray &what) :
	Exception{"Failed to serialize with error: " + what}
{}

void SerializationException::raise() const
{
	throw *this;
}

ExceptionBase *SerializationException::clone() const
{
	auto exc = new SerializationException(QByteArray());
	exc->d = d;
	return exc;
}

DeserializationException::DeserializationException(const QByteArray &what) :
	Exception{"Failed to deserialize with error: " + what}
{}

void DeserializationException::raise() const
{
	throw *this;
}

ExceptionBase *DeserializationException::clone() const
{
	auto exc = new DeserializationException(QByteArray());
	exc->d = d;
	return exc;
}



ExceptionPrivate::ExceptionPrivate(QByteArray msg) :
	message{std::move(msg)},
	trace{ExceptionContext::currentContext()}
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
