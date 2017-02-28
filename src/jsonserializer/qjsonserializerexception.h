#ifndef QJSONSERIALIZEREXCEPTION_H
#define QJSONSERIALIZEREXCEPTION_H

#include "QtJsonSerializer/qjsonserializer_global.h"
#include <QtCore/qexception.h>
#include <QtCore/qstring.h>

//! Exception thrown by QJsonSerializer if something goes wrong
class Q_JSONSERIALIZER_EXPORT QJsonSerializerException : public QException
{
public:
	//! Constructor with error message
	QJsonSerializerException(const QByteArray &what, bool deser);

	//! Specifies whether this exception happend while serializing or deserializing
	bool isDeserializationException() const;

	//! @inherit{std::exception::what}
	const char *what() const noexcept final;

	//! @inherit{QException::raise}
	void raise() const override;
	//! @inherit{QException::clone}
	QException *clone() const override;

protected:
	//! The error message
	QByteArray _what;
	//! the information what kind of exception this is
	bool _isDeser;
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
