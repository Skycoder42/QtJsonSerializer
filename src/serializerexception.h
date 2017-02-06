#ifndef SERIALIZEREXCEPTION_H
#define SERIALIZEREXCEPTION_H

#include <QString>
#include <exception>

class SerializerException : public std::exception
{
public:
	SerializerException(const QString &what, bool deser);

	bool isDeserializationException() const;

	QString qWhat() const;
	const char *what() const noexcept final;

private:
	const QByteArray _what;
	const bool _isDeser;
};

class SerializationException : public SerializerException
{
public:
	inline SerializationException(const QString &what) :
		SerializerException(what, false)
	{}
};

class DeserializationException : public SerializerException
{
public:
	inline DeserializationException(const QString &what) :
		SerializerException(what, true)
	{}
};

#endif // SERIALIZEREXCEPTION_H
