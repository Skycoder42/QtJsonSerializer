#ifndef QTJSONSERIALIZER_EXCEPTION_H
#define QTJSONSERIALIZER_EXCEPTION_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include <QtCore/qstring.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qstack.h>

#if !defined(QT_NO_EXCEPTIONS) && QT_CONFIG(future)
#include <QtCore/qexception.h>
namespace QtJsonSerializer {
//! The exception base class to use for this module
using ExceptionBase = QException;
}
#else
#include <exception>
namespace QtJsonSerializer {
//! The exception base class to use for this module
using ExceptionBase = std::exception;
}
#endif

namespace QtJsonSerializer {

class ExceptionPrivate;
//! Exception thrown by QJsonSerializer if something goes wrong
class Q_JSONSERIALIZER_EXPORT Exception : public ExceptionBase
{
public:
	//! The type of a stack of a property trace (name, type)
	using PropertyTrace = QStack<QPair<QByteArray, QByteArray>>;

	//! Constructor with error message
	Exception(const QByteArray &what);

	//! @inherit{std::exception::what}
	const char *what() const noexcept final;

	//! Returns the error message, without the property trace
	QByteArray message() const;
	//! Returns the property trace
	PropertyTrace propertyTrace() const;

	//! @inherit{QException::raise}
	virtual void raise() const;
	//! @inherit{QException::clone}
	virtual ExceptionBase *clone() const;

protected:
	//! @private
	QSharedPointer<ExceptionPrivate> d;
};

//! Exception thrown by the serializers if something goes wrong while serializing
class Q_JSONSERIALIZER_EXPORT SerializationException : public Exception
{
public:
	//! Constructor with error message
	SerializationException(const QByteArray &what);

	void raise() const override;
	ExceptionBase *clone() const override;
};

//! Exception thrown by the serializers if something goes wrong while deserializing
class Q_JSONSERIALIZER_EXPORT DeserializationException : public Exception
{
public:
	//! Constructor with error message
	DeserializationException(const QByteArray &what);

	void raise() const override;
	ExceptionBase *clone() const override;
};

}

#endif // QTJSONSERIALIZER_EXCEPTION_H
