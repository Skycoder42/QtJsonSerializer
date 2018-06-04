#ifndef QJSONSERIALIZEREXCEPTION_H
#define QJSONSERIALIZEREXCEPTION_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include <QtCore/qexception.h>
#include <QtCore/qstring.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qstack.h>

class QJsonSerializationExceptionPrivate;
//! Exception thrown by QJsonSerializer if something goes wrong
class Q_JSONSERIALIZER_EXPORT QJsonSerializerException : public QException
{
public:
	//! The type of a stack of a property trace
	using PropertyTrace = QStack<QPair<QByteArray, QByteArray>>;

	//! Constructor with error message
	QJsonSerializerException(const QByteArray &what);

	//! @inherit{std::exception::what}
	const char *what() const NOEXCEPT final;

	//! Returns the error message, without the property trace
	QByteArray message() const;
	//! Returns the property trace
	PropertyTrace propertyTrace() const;

	//! @inherit{QException::raise}
	void raise() const override;
	//! @inherit{QException::clone}
	QException *clone() const override;

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
	QException *clone() const override;
};

//! Exception thrown by QJsonSerializer if something goes wrong while deserializing
class Q_JSONSERIALIZER_EXPORT QJsonDeserializationException : public QJsonSerializerException
{
public:
	//! Constructor with error message
	QJsonDeserializationException(const QByteArray &what);

	void raise() const override;
	QException *clone() const override;
};

#endif // QJSONSERIALIZEREXCEPTION_H
