#ifndef QJSONSERIALIZEREXCEPTION_H
#define QJSONSERIALIZEREXCEPTION_H

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

class QJSONSERIALIZERSHARED_EXPORT QJsonSerializerException : public QException
{
public:
	QJsonSerializerException(const QString &what, bool deser);

	bool isDeserializationException() const;

	const char *what() const noexcept final;

	void raise() const override;
	QException *clone() const override;

protected:
	QJsonSerializerException(const QByteArray &what, bool deser);

protected:
	const QByteArray _what;
	const bool _isDeser;
};

class QJSONSERIALIZERSHARED_EXPORT QJsonSerializationException : public QJsonSerializerException
{
public:
	QJsonSerializationException(const QString &what);

	void raise() const override;
	QException *clone() const override;
};

class QJSONSERIALIZERSHARED_EXPORT QJsonDeserializationException : public QJsonSerializerException
{
public:
	QJsonDeserializationException(const QString &what);

	void raise() const override;
	QException *clone() const override;
};

#endif // QJSONSERIALIZEREXCEPTION_H
