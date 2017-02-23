#ifndef SERIALIZEREXCEPTION_H
#define SERIALIZEREXCEPTION_H

#include <QString>
#include <QException>

#ifdef QJSONSERIALIZER_AS_DLL
#if defined(QJSONSERIALIZER_LIBRARY)
#  define QJSONSERIALIZERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QJSONSERIALIZERSHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#define QJSONSERIALIZERSHARED_EXPORT
#endif

class QJSONSERIALIZERSHARED_EXPORT SerializerException : public QException
{
public:
	SerializerException(const QString &what, bool deser);

	bool isDeserializationException() const;

	const char *what() const noexcept final;

	void raise() const override;
	QException *clone() const override;

protected:
	SerializerException(const QByteArray &what, bool deser);

protected:
	const QByteArray _what;
	const bool _isDeser;
};

class QJSONSERIALIZERSHARED_EXPORT SerializationException : public SerializerException
{
public:
	SerializationException(const QString &what);

	void raise() const override;
	QException *clone() const override;
};

class QJSONSERIALIZERSHARED_EXPORT DeserializationException : public SerializerException
{
public:
	DeserializationException(const QString &what);

	void raise() const override;
	QException *clone() const override;
};

#endif // SERIALIZEREXCEPTION_H
