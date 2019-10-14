#ifndef QCBORTYPECONVERTER_H
#define QCBORTYPECONVERTER_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include "QtJsonSerializer/qjsontypeconverter.h"

#include <QtCore/qcborvalue.h>

class Q_JSONSERIALIZER_EXPORT QCborTypeConverter : public QJsonTypeConverter
{
public:
	static constexpr QCborTag NoTag = static_cast<QCborTag>(-1);

	//! Helper class passed to the type converter by the serializer. Do not implement yourself
	class Q_JSONSERIALIZER_EXPORT SerializationHelper : public QJsonTypeConverter::SerializationHelper
	{
		Q_DISABLE_COPY(SerializationHelper)
	public:
		SerializationHelper();

		//! Serialize a subvalue, represented by a meta property
		virtual QCborValue serializeCborSubtype(const QMetaProperty &property, const QVariant &value) const = 0;
		//! Serialize a subvalue, represented by a type id
		virtual QCborValue serializeCborSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint = {}) const = 0;
		//! Deserialize a subvalue, represented by a meta property
		virtual QVariant deserializeCborSubtype(const QMetaProperty &property, const QCborValue &value, QObject *parent) const = 0;
		//! Deserialize a subvalue, represented by a type id
		virtual QVariant deserializeCborSubtype(int propertyType, const QCborValue &value, QObject *parent, const QByteArray &traceHint = {}) const = 0;
	};

	virtual QList<QCborValue::Type> cborTypes() const = 0;
	virtual QList<QCborTag> allowedCborTags() const;
	bool canDeserialize(QCborValue::Type valueType, QCborTag tag) const;
	virtual QCborValue serializeCbor(int propertyType, const QVariant &value, const SerializationHelper *helper) const = 0;
	virtual QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const SerializationHelper *helper) const = 0;

	// QJsonTypeConverter interface
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const override;

protected:
	virtual QList<QJsonValue::Type> cborToJsonType(QCborValue::Type type) const;
};

//! A factory interface to create instances of QCborTypeConverterFactory
class Q_JSONSERIALIZER_EXPORT QCborTypeConverterFactory : public QJsonTypeConverterFactory
{
public:
	QCborTypeConverterFactory();

	//! @copydoc QCborTypeConverter::canDeserialize
	bool canDeserialize(QCborValue::Type valueType, QCborTag tag) const;

	//! The primary factory method to create converters
	virtual QSharedPointer<QCborTypeConverter> createCborConverter() const = 0;

	QSharedPointer<QJsonTypeConverter> createConverter() const final;
};

//! A template implementation of QCborTypeConverterFactory to generically create simple converters
template <typename TConverter, int Priority = QJsonTypeConverter::Priority::Standard>
class QCborTypeConverterStandardFactory : public QCborTypeConverterFactory
{
public:
	QSharedPointer<QCborTypeConverter> createCborConverter() const override;
};

// ------------- GENERIC IMPLEMENTATION -------------

template<typename TConverter, int Priority>
QSharedPointer<QCborTypeConverter> QCborTypeConverterStandardFactory<TConverter, Priority>::createCborConverter() const
{
	auto converter = QSharedPointer<TConverter>::create();
	converter->setPriority(Priority);
	return converter;
}

#endif // QCBORTYPECONVERTER_H
