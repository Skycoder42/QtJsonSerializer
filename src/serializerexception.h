#ifndef SERIALIZEREXCEPTION_H
#define SERIALIZEREXCEPTION_H

#include <QString>
#include <exception>

#ifdef QJSONSERIALIZER_AS_DLL
#if defined(QJSONSERIALIZER_LIBRARY)
#  define QJSONSERIALIZERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QJSONSERIALIZERSHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#define QJSONSERIALIZERSHARED_EXPORT
#endif

class QJSONSERIALIZERSHARED_EXPORT SerializerException : public std::exception
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

class QJSONSERIALIZERSHARED_EXPORT SerializationException : public SerializerException
{
public:
	SerializationException(const QString &what);
};

class QJSONSERIALIZERSHARED_EXPORT DeserializationException : public SerializerException
{
public:
	DeserializationException(const QString &what);
};

#endif // SERIALIZEREXCEPTION_H
