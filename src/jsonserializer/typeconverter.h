#ifndef QTJSONSERIALIZER_TYPECONVERTER_H
#define QTJSONSERIALIZER_TYPECONVERTER_H

#include "QtJsonSerializer/qtjsonserializer_global.h"

#include <type_traits>
#include <limits>

#include <QtCore/qmetatype.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qcborvalue.h>
#include <QtCore/qvariant.h>
#include <QtCore/qsharedpointer.h>

namespace QtJsonSerializer {

//! Interface to extract data from any generic container
class Q_JSONSERIALIZER_EXPORT TypeExtractor
{
	Q_DISABLE_COPY(TypeExtractor)

public:
	TypeExtractor();
	virtual ~TypeExtractor();

	//! Returns the identifier of the general type this extractor is for
	virtual QByteArray baseType() const = 0;
	//! Returns an ordered list of subtypes found in types handled by this extractor
	virtual QList<int> subtypes() const = 0;
	//! Extracts the data of a value of the extractors type at the given index
	virtual QVariant extract(const QVariant &value, int index = -1) const = 0;
	//! Emplaces the value into the target of the extractors type at the given index
	virtual void emplace(QVariant &target, const QVariant &value, int index = -1) const = 0;
};

class TypeConverterPrivate;
//! An interface to create custom serializer type converters
class Q_JSONSERIALIZER_EXPORT TypeConverter
{
	Q_DISABLE_COPY(TypeConverter)
public:
	//! A placeholder tag to be used if no tag is expected/allowed/given
	static constexpr auto NoTag = static_cast<QCborTag>(std::numeric_limits<std::underlying_type_t<QCborTag>>::max());

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

	//! The possible results of canDeserialize(), used internally only
	enum DeserializationCapabilityResult : int {
		Positive = 1, //!< The converter can deserialize the given data
		Guessed = 2, //!< The converter guessed a type for the given data
		Negative = -1, //!< The converter cannot deserialize the given data
		WrongTag = -2 //!< The converter could deserialize the given data, but the tag does not match
	};

	//! Helper class passed to the type converter by the serializer. Do not implement yourself
	class Q_JSONSERIALIZER_EXPORT SerializationHelper
	{
		Q_DISABLE_COPY(SerializationHelper)
	public:
		SerializationHelper();
		virtual ~SerializationHelper();

		//! Returns true, if de/serializing to JSON, and false for CBOR
		virtual bool jsonMode() const = 0;
		//! Returns a property from the serializer
		virtual QVariant getProperty(const char *name) const = 0;
		//! Returns a tag registered for the given metaTypeId
		virtual QCborTag typeTag(int metaTypeId) const = 0;
		//! Returns a reference to an extractor for the given type, or nullptr
		virtual QSharedPointer<const TypeExtractor> extractor(int metaTypeId) const = 0;

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
	TypeConverter();
	//! Destructor
	virtual ~TypeConverter();

	//! The name of the converter. Used for debugging purpose
	virtual QByteArray name() const = 0;

	//! Returns the priority of this converter
	int priority() const;
	//! Sets the priority of this converter
	void setPriority(int priority);

	//! Returns the helper associated with this converter
	const SerializationHelper *helper() const;
	//! @private
	void setHelper(const SerializationHelper *helper);

	//! Returns true, if this implementation can convert the given type
	virtual bool canConvert(int metaTypeId) const = 0;
	//! Returns a list of allowed tags for the given type
	virtual QList<QCborTag> allowedCborTags(int metaTypeId) const;
	//! Returns a list of allowed types for the given type and tag
	virtual QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const = 0;
	//! Returns a type guessed from the tag and data, that is supported by the converter
	virtual int guessType(QCborTag tag, QCborValue::Type dataType) const;
	//! @private
	DeserializationCapabilityResult canDeserialize(int &metaTypeId,
												   QCborTag tag,
												   QCborValue::Type dataType) const;

	//! Called by the serializer to serializer your given type to CBOR
	virtual QCborValue serialize(int propertyType, const QVariant &value) const = 0;
	//! Called by the serializer to deserializer your given type from CBOR
	virtual QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const = 0;
	//! Called by the serializer to deserializer your given type from JSON
	virtual QVariant deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const;

private:
	QScopedPointer<TypeConverterPrivate> d;

	void mapTypesToJson(QList<QCborValue::Type> &typeList) const;
};

//! Macro to implement the TypeConverter::name method in a subclass
#define QT_JSONSERIALIZER_TYPECONVERTER_NAME(className) inline QByteArray name() const override { \
	static_assert(std::is_same_v<className, std::decay_t<decltype(*this)>>); \
	return QByteArrayLiteral(#className); \
}

//! A factory interface to create instances of QJsonTypeConverters
class Q_JSONSERIALIZER_EXPORT TypeConverterFactory
{
	Q_DISABLE_COPY(TypeConverterFactory)

public:
	TypeConverterFactory();
	virtual ~TypeConverterFactory();

	//! The primary factory method to create converters
	virtual QSharedPointer<TypeConverter> createConverter() const = 0;
};

//! A template implementation of TypeConverterFactory to generically create simply converters
template <typename TConverter, int OverwritePriority = TypeConverter::Priority::Standard>
class TypeConverterStandardFactory : public TypeConverterFactory
{
public:
	QSharedPointer<TypeConverter> createConverter() const override;
};

// ------------- GENERIC IMPLEMENTATION -------------

template<typename TConverter, int OverwritePriority>
QSharedPointer<TypeConverter> TypeConverterStandardFactory<TConverter, OverwritePriority>::createConverter() const
{
	auto converter = QSharedPointer<TConverter>::create();
	if (OverwritePriority != TypeConverter::Priority::Standard)
		converter->setPriority(OverwritePriority);
	return converter;
}

}

#endif // QTJSONSERIALIZER_TYPECONVERTER_H
