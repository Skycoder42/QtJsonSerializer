#ifndef QJSONSERIALIZERBASE_H
#define QJSONSERIALIZERBASE_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include "QtJsonSerializer/qjsonserializerexception.h"
#include "QtJsonSerializer/qjsontypeconverter.h"
#include "QtJsonSerializer/qjsonserializer_helpertypes.h"
#include "QtJsonSerializer/qcontainerwriters.h"
#include "QtJsonSerializer/qjsontypeextractors.h"

#include <tuple>
#include <optional>
#include <variant>

#include <QtCore/qobject.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qcborvalue.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qpointer.h>
#include <QtCore/qlist.h>
#include <QtCore/qlinkedlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qset.h>
#include <QtCore/qqueue.h>
#include <QtCore/qstack.h>
#include <QtCore/qhash.h>
#include <QtCore/qmap.h>

class QJsonSerializerBasePrivate;
//! A class to serializer and deserializer c++ classes to and from JSON
class Q_JSONSERIALIZER_EXPORT QJsonSerializerBase : public QObject, protected QJsonTypeConverter::SerializationHelper
{
	Q_OBJECT

	//! Specifies, whether null for value types is allowed or not
	Q_PROPERTY(bool allowDefaultNull READ allowDefaultNull WRITE setAllowDefaultNull NOTIFY allowDefaultNullChanged)
	//! Specifies, whether the `objectName` property of QObjects should be serialized
	Q_PROPERTY(bool keepObjectName READ keepObjectName WRITE setKeepObjectName NOTIFY keepObjectNameChanged)
	//! Specifies, whether enums should be serialized as integer or as string
	Q_PROPERTY(bool enumAsString READ enumAsString WRITE setEnumAsString NOTIFY enumAsStringChanged)
	Q_PROPERTY(bool versionAsString READ versionAsString WRITE setVersionAsString NOTIFY versionAsStringChanged)
	//! Specify whether serializing a QLocale should use the bcp47 format
	Q_PROPERTY(bool useBcp47Locale READ useBcp47Locale WRITE setUseBcp47Locale NOTIFY useBcp47LocaleChanged)
	//! Specify how strictly the serializer should verify data when deserializing
	Q_PROPERTY(ValidationFlags validationFlags READ validationFlags WRITE setValidationFlags NOTIFY validationFlagsChanged)
	//! Specify how the serializer should treat polymorphism for QObject classes
	Q_PROPERTY(Polymorphing polymorphing READ polymorphing WRITE setPolymorphing NOTIFY polymorphingChanged)
	//! Specify how multi maps and sets should be serialized
	Q_PROPERTY(MultiMapMode multiMapMode READ multiMapMode WRITE setMultiMapMode NOTIFY multiMapModeChanged)
	Q_PROPERTY(bool ignoreStoredAttribute READ ignoresStoredAttribute WRITE setIgnoreStoredAttribute NOTIFY ignoreStoredAttributeChanged)

public:
	//! Flags to specify how strict the serializer should validate when deserializing
	enum class ValidationFlag {
		StandardValidation = 0x00, //!< Do not perform extra validation, only make sure types are valid and compatible
		NoExtraProperties = 0x01, //!< Make sure the json does not contain any properties that are not in the type to deserialize it to
		AllProperties = 0x02, //!< Make sure all properties of the type have a value in the deserialized json data
		StrictBasicTypes = 0x04, //!< Make shure basic types (string, int, ...) are actually of those types, instead of accepting all that are convertible

		FullPropertyValidation = (NoExtraProperties | AllProperties), //!< Validate properties are exactly the same as declared
		FullValidation2 = (FullPropertyValidation | StrictBasicTypes), //!< Validate everything

		FullValidation Q_DECL_ENUMERATOR_DEPRECATED_X("Use QJsonSerializer::FullValidation2 or QJsonSerializer::FullPropertyValidation instead") = (NoExtraProperties | AllProperties)
	};
	Q_DECLARE_FLAGS(ValidationFlags, ValidationFlag)
	Q_FLAG(ValidationFlags)

	//! Enum to specify the modes of polymorphism
	enum class Polymorphing {
		Disabled, //!< Do not serialize polymorphic and ignore information about classes in json
		Enabled, //!< Use polymorphism where declared by the classes/json
		Forced //!< Treat every object polymorphic, and required the class information to be present in json
	};
	Q_ENUM(Polymorphing)

	//! Enum to specify how multi maps and sets should be serialized
	enum class MultiMapMode {
		Map, //!< Store them as json object, with each element beeing a json array containing the actual values
		List,  //!< Store a list of pairs, where for each pair the first element is the key and the second the value
		DenseMap
	};
	Q_ENUM(MultiMapMode)

	template<typename TType, typename TExtractor>
	static void registerExtractor();
	static void registerExtractor(int metaTypeId, const QSharedPointer<QJsonTypeExtractor> &extractor);

	//! Registers a custom type for list converisons
	template<typename T>
	static inline void registerListConverters();
	//! Registers a custom type for set converisons
	template<typename T>
	static inline void registerSetConverters();
	//! Registers a custom type for map converisons
	template<typename TKey, typename TValue, bool mapTypes = true, bool hashTypes = true>
	static inline void registerMapConverters();
	//! Registers a custom type for QSharedPointer and QPointer converisons
	template<typename T>
	static inline void registerPointerConverters();
	//! Registers a custom type for list, set map and optional converisons. Also include pointer converters, if applicable
	template<typename T>
	static inline void registerBasicConverters();
	//! Registers two types for pair conversion
	template<typename T, typename U>
	static inline void registerPairConverters();
	//! Registers a number of types for std::tuple conversion
	template<typename... TArgs>
	static inline void registerTupleConverters();
	//! Registers a custom type for std::optional converisons
	template<typename T>
	static inline void registerOptionalConverters();
	//! Registers a custom type for std::variant converisons
	template<typename... TArgs>
	static inline void registerVariantConverters();

	virtual std::variant<QCborValue, QJsonValue> serializeGeneric(const QVariant &value) const = 0;
	virtual QVariant deserializeGeneric(const std::variant<QCborValue, QJsonValue> &value, int metaTypeId, QObject *parent = nullptr) const = 0;

	//! @readAcFn{QJsonSerializer::allowDefaultNull}
	bool allowDefaultNull() const;
	//! @readAcFn{QJsonSerializer::keepObjectName}
	bool keepObjectName() const;
	//! @readAcFn{QJsonSerializer::enumAsString}
	bool enumAsString() const;
	//! @readAcFn{QJsonSerializer::versionAsString}
	bool versionAsString() const;
	//! @readAcFn{QJsonSerializer::useBcp47Locale}
	bool useBcp47Locale() const;
	//! @readAcFn{QJsonSerializer::validationFlags}
	ValidationFlags validationFlags() const;
	//! @readAcFn{QJsonSerializer::polymorphing}
	Polymorphing polymorphing() const;
	//! @readAcFn{QJsonSerializer::multiMapMode}
	MultiMapMode multiMapMode() const;
	//! @readAcFn{QJsonSerializer::ignoreStoredAttribute}
	bool ignoresStoredAttribute() const;

	//! Globally registers a converter factory to provide converters for all QJsonSerializer instances
	template <typename TConverter, int Priority = QJsonTypeConverter::Priority::Standard>
	static void addJsonTypeConverterFactory();
	//! @copybrief QJsonSerializer::addJsonTypeConverterFactory()
	static void addJsonTypeConverterFactory(QJsonTypeConverterFactory *factory);

	//! Adds a custom type converter to this serializer
	template <typename TConverter>
	void addJsonTypeConverter();
	//! @copybrief QJsonSerializer::addJsonTypeConverter()
	void addJsonTypeConverter(const QSharedPointer<QJsonTypeConverter> &converter);

public Q_SLOTS:
	//! @writeAcFn{QJsonSerializer::allowDefaultNull}
	void setAllowDefaultNull(bool allowDefaultNull);
	//! @writeAcFn{QJsonSerializer::keepObjectName}
	void setKeepObjectName(bool keepObjectName);
	//! @writeAcFn{QJsonSerializer::enumAsString}
	void setEnumAsString(bool enumAsString);
	//! @writeAcFn{QJsonSerializer::versionAsString}
	void setVersionAsString(bool versionAsString);
	//! @writeAcFn{QJsonSerializer::useBcp47Locale}
	void setUseBcp47Locale(bool useBcp47Locale);
	//! @writeAcFn{QJsonSerializer::validationFlags}
	void setValidationFlags(ValidationFlags validationFlags);
	//! @writeAcFn{QJsonSerializer::polymorphing}
	void setPolymorphing(Polymorphing polymorphing);
	//! @writeAcFn{QJsonSerializer::multiMapMode}
	void setMultiMapMode(MultiMapMode multiMapMode);
	//! @writeAcFn{QJsonSerializer::ignoreStoredAttribute}
	void setIgnoreStoredAttribute(bool ignoreStoredAttribute);

Q_SIGNALS:
	//! @notifyAcFn{QJsonSerializer::allowDefaultNull}
	void allowDefaultNullChanged(bool allowDefaultNull, QPrivateSignal);
	//! @notifyAcFn{QJsonSerializer::keepObjectName}
	void keepObjectNameChanged(bool keepObjectName, QPrivateSignal);
	//! @notifyAcFn{QJsonSerializer::enumAsString}
	void enumAsStringChanged(bool enumAsString, QPrivateSignal);
	//! @notifyAcFn{QJsonSerializer::versionAsString}
	void versionAsStringChanged(bool versionAsString, QPrivateSignal);
	//! @notifyAcFn{QJsonSerializer::useBcp47Locale}
	void useBcp47LocaleChanged(bool useBcp47Locale, QPrivateSignal);
	//! @notifyAcFn{QJsonSerializer::validationFlags}
	void validationFlagsChanged(ValidationFlags validationFlags, QPrivateSignal);
	//! @notifyAcFn{QJsonSerializer::polymorphing}
	void polymorphingChanged(Polymorphing polymorphing, QPrivateSignal);
	//! @notifyAcFn{QJsonSerializer::multiMapMode}
	void multiMapModeChanged(MultiMapMode multiMapMode, QPrivateSignal);
	//! @notifyAcFn{QJsonSerializer::ignoreStoredAttribute}
	void ignoreStoredAttributeChanged(bool ignoreStoredAttribute, QPrivateSignal);

protected:
	explicit QJsonSerializerBase(QObject *parent = nullptr);
	explicit QJsonSerializerBase(QJsonSerializerBasePrivate &dd, QObject *parent);

	virtual QList<int> typesForTag(QCborTag tag) const = 0;

	// protected implementation -> internal use for the type converters
	QVariant getProperty(const char *name) const override;
	QSharedPointer<const QJsonTypeExtractor> extractor(int metaTypeId) const override;
	QCborValue serializeSubtype(const QMetaProperty &property, const QVariant &value) const override;
	QCborValue serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const override;
	QVariant deserializeSubtype(const QMetaProperty &property, const QCborValue &value, QObject *parent) const override;
	QVariant deserializeSubtype(int propertyType, const QCborValue &value, QObject *parent, const QByteArray &traceHint) const override;

	QCborValue serializeVariant(int propertyType, const QVariant &value) const;
	QVariant deserializeVariant(int propertyType, const QCborValue &value, QObject *parent, bool skipConversion = false) const;

private:
	Q_DECLARE_PRIVATE(QJsonSerializerBase)

	static void registerInverseTypedefImpl(int typeId, const char *normalizedTypeName);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QJsonSerializerBase::ValidationFlags)

Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(QMultiMap)
Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(QMultiHash)

//! A macro the mark a class as polymorphic
#define Q_JSON_POLYMORPHIC(x) \
	static_assert(std::is_same<decltype(x), bool>::value, "x must be bool"); \
	Q_CLASSINFO("polymorphic", #x)

// ------------- Generic Implementation -------------

template<typename TType, typename TExtractor>
void QJsonSerializerBase::registerExtractor()
{
	registerExtractor(qMetaTypeId<TType>(), QSharedPointer<TExtractor>::create());
}

template<typename T>
void QJsonSerializerBase::registerListConverters()
{
	QSequentialWriter::registerWriter<QList, T>();
	QSequentialWriter::registerWriter<QLinkedList, T>();
	QSequentialWriter::registerWriter<QVector, T>();
	QSequentialWriter::registerWriter<QStack, T>();
	QSequentialWriter::registerWriter<QQueue, T>();
}

template<typename T>
void QJsonSerializerBase::registerSetConverters()
{
	QSequentialWriter::registerWriter<QSet, T>();
}

template<typename TKey, typename TValue, bool mapTypes, bool hashTypes>
void QJsonSerializerBase::registerMapConverters()
{
	if constexpr (mapTypes) {
		QAssociativeWriter::registerWriter<QMap, TKey, TValue>();
		QAssociativeWriter::registerWriter<QMultiMap, TKey, TValue>();
	}
	if constexpr (hashTypes) {
		QAssociativeWriter::registerWriter<QHash, TKey, TValue>();
		QAssociativeWriter::registerWriter<QMultiHash, TKey, TValue>();
	}
}

template<typename T>
void QJsonSerializerBase::registerPointerConverters()
{
	static_assert(std::is_base_of_v<QObject, T>, "T must inherit QObject");
	QMetaType::registerConverter<QSharedPointer<QObject>, QSharedPointer<T>>([](const QSharedPointer<QObject> &object) -> QSharedPointer<T> {
		return object.objectCast<T>();
	});
	QMetaType::registerConverter<QSharedPointer<T>, QSharedPointer<QObject>>([](const QSharedPointer<T> &object) -> QSharedPointer<QObject> {
		return object.template staticCast<QObject>();  // must work, because of static assert
	});
	QMetaType::registerConverter<QPointer<QObject>, QPointer<T>>([](const QPointer<QObject> &object) -> QPointer<T> {
		return qobject_cast<T*>(object.data());
	});
	QMetaType::registerConverter<QPointer<T>, QPointer<QObject>>([](const QPointer<T> &object) -> QPointer<QObject> {
		return static_cast<QObject*>(object.data());
	});
}

template<typename T>
void QJsonSerializerBase::registerBasicConverters()
{
	if constexpr (std::is_base_of_v<QObject, T>) {
		registerBasicConverters<T*>() &
			(registerPointerConverters<T>() &&
			 (registerBasicConverters<QSharedPointer<T>>() &
			  registerBasicConverters<QPointer<T>>()));
	} else {
		registerListConverters<T>() &
			registerSetConverters<T>() &
			registerMapConverters<QString, T>();
	}
}

template<typename T1, typename T2>
void QJsonSerializerBase::registerPairConverters()
{
	registerExtractor<QPair<T1, T2>, QJsonTypeExtractors::PairExtractor<QPair, T1, T2>>();
	registerExtractor<std::pair<T1, T2>, QJsonTypeExtractors::PairExtractor<std::pair, T1, T2>>();
}

template<typename... TArgs>
void QJsonSerializerBase::registerTupleConverters()
{
	registerExtractor<std::tuple<TArgs...>, QJsonTypeExtractors::TupleExtractor<TArgs...>>();
}

template<typename T>
void QJsonSerializerBase::registerOptionalConverters()
{
	registerExtractor<std::optional<T>, QJsonTypeExtractors::OptionalExtractor<T>>();
}

template<typename... TArgs>
void QJsonSerializerBase::registerVariantConverters()
{
	registerExtractor<std::variant<TArgs...>, QJsonTypeExtractors::VariantExtractor<TArgs...>>();
}

template<typename TConverter, int Priority>
void QJsonSerializerBase::addJsonTypeConverterFactory()
{
	static_assert(std::is_base_of<QJsonTypeConverter, TConverter>::value, "T must implement QJsonTypeConverter");
	addJsonTypeConverterFactory(new QJsonTypeConverterStandardFactory<TConverter, Priority>{});
}

template<typename TConverter>
void QJsonSerializerBase::addJsonTypeConverter()
{
	static_assert(std::is_base_of<QJsonTypeConverter, TConverter>::value, "T must implement QJsonTypeConverter");
	addJsonTypeConverter(QSharedPointer<TConverter>::create());
}

//! @file qjsonserializerbase.h The QJsonSerializerBase header file
#endif // QJSONSERIALIZERBASE_H
