#ifndef QJSONTYPECONVERTER_H
#define QJSONTYPECONVERTER_H

#include "QtJsonSerializer/qtjsonserializer_global.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qcborvalue.h>
#include <QtCore/qvariant.h>
#include <QtCore/qsharedpointer.h>

class QJsonTypeConverterPrivate;
//! An interface to create custom serializer type converters
class Q_JSONSERIALIZER_EXPORT QJsonTypeConverter
{
	Q_DISABLE_COPY(QJsonTypeConverter)
public:
	static constexpr QCborTag NoTag = static_cast<QCborTag>(-1);

	//! Sample values for a priority value (default converters are mostly Standard and are guaranteed to be between Low and High)
	enum Priority : int {
		ExtremlyLow = -0x00FFFFFF,
		VeryLow = -0x0000FFFF,
		Low = -0x000000FF,
		Standard = 0x00000000,
		High = 0x000000FF,
		VeryHigh = 0x0000FFFF,
		ExtremlyHigh = 0x00FFFFFF
	};

	enum DeserializationCapabilityResult : int {
		Positive = 1,
		Guessed = 2,
		Negative = -1,
		WrongTag = -2
	};

	//! Helper class passed to the type converter by the serializer. Do not implement yourself
	class Q_JSONSERIALIZER_EXPORT SerializationHelper
	{
		Q_DISABLE_COPY(SerializationHelper)
	public:
		SerializationHelper();
		virtual ~SerializationHelper();

		virtual bool jsonMode() const = 0;
		//! Returns a property from the serializer
		virtual QVariant getProperty(const char *name) const = 0;
		virtual QCborTag typeTag(int metaTypeId) const = 0;
		virtual QByteArray getCanonicalTypeName(int propertyType) const = 0;

		//! Serialize a subvalue, represented by a meta property
		virtual QCborValue serializeSubtype(const QMetaProperty &property, const QVariant &value) const = 0;
		//! Serialize a subvalue, represented by a type id
		virtual QCborValue serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint = {}) const = 0;
		//! Deserialize a subvalue, represented by a meta property
		virtual QVariant deserializeSubtype(const QMetaProperty &property, const QCborValue &value, QObject *parent) const = 0;
		//! Deserialize a subvalue, represented by a type id
		virtual QVariant deserializeSubtype(int propertyType, const QCborValue &value, QObject *parent, const QByteArray &traceHint = {}) const = 0;
	};

	//! Constructor
	QJsonTypeConverter();
	//! Destructor
	virtual ~QJsonTypeConverter();

	//! Returns the priority of this converter
	int priority() const;
	//! Sets the priority of this converter
	void setPriority(int priority);

	//! Returns true, if this implementation can convert the given type
	virtual bool canConvert(int metaTypeId) const = 0;
	virtual QList<QCborTag> allowedCborTags(int metaTypeId) const;
	virtual QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const = 0;
	virtual int guessType(QCborTag tag, QCborValue::Type dataType) const;
	DeserializationCapabilityResult canDeserialize(int &metaTypeId,
												   QCborTag tag,
												   QCborValue::Type dataType,
												   const SerializationHelper *helper) const;

	//! Called by the serializer to serializer your given type
	virtual QCborValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const = 0;
	//! Called by the deserializer to serializer your given type
	virtual QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const SerializationHelper *helper) const = 0;
	virtual QVariant deserializeJson(int propertyType, const QCborValue &value, QObject *parent, const SerializationHelper *helper) const;

private:
	QScopedPointer<QJsonTypeConverterPrivate> d;

	void mapTypesToJson(QList<QCborValue::Type> &typeList) const;
};

class QCborTypeConverterFactory;
//! A factory interface to create instances of QJsonTypeConverters
class Q_JSONSERIALIZER_EXPORT QJsonTypeConverterFactory
{
	Q_DISABLE_COPY(QJsonTypeConverterFactory)

public:
	QJsonTypeConverterFactory();
	virtual ~QJsonTypeConverterFactory();

	//! @copydoc QJsonTypeConverter::priority
	int priority() const;
	//! @copydoc QJsonTypeConverter::canConvert
	bool canConvert(int metaTypeId) const;
	//! @copydoc QJsonTypeConverter::canDeserialize
	QJsonTypeConverter::DeserializationCapabilityResult canDeserialize(int &metaTypeId,
																	   QCborTag tag,
																	   QCborValue::Type dataType,
																	   const QJsonTypeConverter::SerializationHelper *helper) const;

	//! The primary factory method to create converters
	virtual QSharedPointer<QJsonTypeConverter> createConverter() const = 0;

private:
	friend class QCborTypeConverterFactory;
	mutable QSharedPointer<const QJsonTypeConverter> _statusConverter;
};

//! A template implementation of QJsonTypeConverterFactory to generically create simply converters
template <typename TConverter, int OverwritePriority = QJsonTypeConverter::Priority::Standard>
class QJsonTypeConverterStandardFactory : public QJsonTypeConverterFactory
{
public:
	QSharedPointer<QJsonTypeConverter> createConverter() const override;
};

// ------------- GENERIC IMPLEMENTATION -------------

template<typename TConverter, int OverwritePriority>
QSharedPointer<QJsonTypeConverter> QJsonTypeConverterStandardFactory<TConverter, OverwritePriority>::createConverter() const
{
	auto converter = QSharedPointer<TConverter>::create();
	if (OverwritePriority != QJsonTypeConverter::Priority::Standard)
		converter->setPriority(OverwritePriority);
	return converter;
}

#endif // QJSONTYPECONVERTER_H
