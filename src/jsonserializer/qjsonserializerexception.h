#ifndef QJSONSERIALIZEREXCEPTION_H
#define QJSONSERIALIZEREXCEPTION_H

#include "QtJsonSerializer/qjsonserializer_global.h"
#include <QtCore/qexception.h>
#include <QtCore/qstring.h>

class Q_JSONSERIALIZER_EXPORT QJsonSerializerException : public QException
{
public:
	QJsonSerializerException(const QByteArray &what, bool deser);

	bool isDeserializationException() const;

	const char *what() const noexcept final;

	void raise() const override;
	QException *clone() const override;

protected:
	QByteArray _what;
	bool _isDeser;
};

class Q_JSONSERIALIZER_EXPORT QJsonSerializationException : public QJsonSerializerException
{
public:
	QJsonSerializationException(const QByteArray &what);

	void raise() const override;
	QException *clone() const override;
};

class Q_JSONSERIALIZER_EXPORT QJsonDeserializationException : public QJsonSerializerException
{
public:
	QJsonDeserializationException(const QByteArray &what);

	void raise() const override;
	QException *clone() const override;
};

#endif // QJSONSERIALIZEREXCEPTION_H
