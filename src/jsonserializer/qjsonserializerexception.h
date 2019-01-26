#ifndef QJSONSERIALIZEREXCEPTION_H
#define QJSONSERIALIZEREXCEPTION_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include <QtCore/qstring.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qstack.h>

#if !defined(QT_NO_EXCEPTIONS) && QT_CONFIG(future)
#include <QtCore/qexception.h>
#define QT_JSONSERIALIZER_EXCEPTION_BASE QException
#define QT_JSONSERIALIZER_EXCEPTION_OR override
#else
#include <exception>
#define QT_JSONSERIALIZER_EXCEPTION_BASE std::exception
#define QT_JSONSERIALIZER_EXCEPTION_OR
#endif

class QJsonSerializationExceptionPrivate;
//! Exception thrown by QJsonSerializer if something goes wrong
class Q_JSONSERIALIZER_EXPORT QJsonSerializerException : public QT_JSONSERIALIZER_EXCEPTION_BASE
{
public:
	//! The type of a stack of a property trace (name, type)
	using PropertyTrace = QStack<QPair<QByteArray, QByteArray>>;

	//! Constructor with error message
	QJsonSerializerException(const QByteArray &what);

	//! @inherit{std::exception::what}
	const char *what() const noexcept final;

	//! Returns the error message, without the property trace
	QByteArray message() const;
	//! Returns the property trace
	PropertyTrace propertyTrace() const;

	//! @inherit{QException::raise}
	virtual void raise() const QT_JSONSERIALIZER_EXCEPTION_OR;
	//! @inherit{QException::clone}
	virtual QT_JSONSERIALIZER_EXCEPTION_BASE *clone() const QT_JSONSERIALIZER_EXCEPTION_OR;

protected:
	//! @private
	QSharedPointer<QJsonSerializationExceptionPrivate> d;
};

//! Exception thrown by QJsonSerializer if something goes wrong while serializing
class Q_JSONSERIALIZER_EXPORT QJsonSerializationException : public QJsonSerializerException
{
public:
	//! Constructor with error message
	QJsonSerializationException(const QByteArray &what);

	void raise() const override;
	QT_JSONSERIALIZER_EXCEPTION_BASE *clone() const override;
};

//! Exception thrown by QJsonSerializer if something goes wrong while deserializing
class Q_JSONSERIALIZER_EXPORT QJsonDeserializationException : public QJsonSerializerException
{
public:
	//! Constructor with error message
	QJsonDeserializationException(const QByteArray &what);

	void raise() const override;
	QT_JSONSERIALIZER_EXCEPTION_BASE *clone() const override;
};

#endif // QJSONSERIALIZEREXCEPTION_H
