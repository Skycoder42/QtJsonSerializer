#ifndef QJSONSERIALIZER_H
#define QJSONSERIALIZER_H

#include <type_traits>

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include "QtJsonSerializer/qjsonserializerexception.h"
#include "QtJsonSerializer/qjsonserializer_helpertypes.h"
#include "QtJsonSerializer/qjsontypeconverter.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qpointer.h>
#include <QtCore/qlist.h>
#include <QtCore/qlinkedlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qset.h>
#include <QtCore/qhash.h>
#include <QtCore/qmap.h>

class QJsonSerializerPrivate;
//! A class to serializer and deserializer c++ classes to and from JSON
class Q_JSONSERIALIZER_EXPORT QJsonSerializer : public QObject, protected QJsonTypeConverter::SerializationHelper
{
	Q_OBJECT

	//! Specifies, whether null for value types is allowed or not
	Q_PROPERTY(bool allowDefaultNull READ allowDefaultNull WRITE setAllowDefaultNull)
	//! Specifies, whether the `objectName` property of QObjects should be serialized
	Q_PROPERTY(bool keepObjectName READ keepObjectName WRITE setKeepObjectName)
	//! Specifies, whether enums should be serialized as integer or as string
	Q_PROPERTY(bool enumAsString READ enumAsString WRITE setEnumAsString)
	//! Specify whether deserializing a QByteArray should verify the data as base64 instead of silent discarding
	Q_PROPERTY(bool validateBase64 READ validateBase64 WRITE setValidateBase64)
	//! Specify whether serializing a QLocale should use the bcp47 format
	Q_PROPERTY(bool useBcp47Locale READ useBcp47Locale WRITE setUseBcp47Locale)
	//! Specify how strictly the serializer should verify data when deserializing
	Q_PROPERTY(ValidationFlags validationFlags READ validationFlags WRITE setValidationFlags)
	//! Specify how the serializer should treat polymorphism for QObject classes
	Q_PROPERTY(Polymorphing polymorphing READ polymorphing WRITE setPolymorphing)
	Q_PROPERTY(MultiMapMode multiMapMode READ multiMapMode WRITE setMultiMapMode)

public:
	//! Flags to specify how strict the serializer should validate when deserializing
	enum ValidationFlag {
		StandardValidation = 0x00,//!< Do not perform extra validation, only make sure types are valid and compatible
		NoExtraProperties = 0x01,//!< Make sure the json does not contain any properties that are not in the type to deserialize it to
		AllProperties = 0x02,//!< Make sure all properties of the type have a value in the deserialized json data
		FullValidation = (NoExtraProperties | AllProperties)//!< Validate everything
	};
	Q_DECLARE_FLAGS(ValidationFlags, ValidationFlag)
	Q_FLAG(ValidationFlags)

	//! Enum to specify the modes of polymorphism
	enum Polymorphing {
		Disabled,//!< Do not serialize polymorphic and ignore information about classes in json
		Enabled,//!< Use polymorphism where declared by the classes/json
		Forced//!< Treat every object polymorphic, and required the class information to be present in json
	};
	Q_ENUM(Polymorphing)

	enum class MultiMapMode {
		Map,
		List
	};
	Q_ENUM(MultiMapMode)

	//! Constructor
	explicit QJsonSerializer(QObject *parent = nullptr);
	~QJsonSerializer() override;

	//! Registers a the original name of a declared typedef
	template<typename T>
	static void registerInverseTypedef(const char *typeName);

	template <template<typename> class TContainer, typename TClass, typename TAppendRet = void>
	static bool registerListContainerConverters(TAppendRet (TContainer<TClass>::*appendMethod)(const TClass &) = &TContainer<TClass>::append,
												void (TContainer<TClass>::*reserveMethod)(int) = &TContainer<TClass>::reserve);
	template <template<typename, typename> class TContainer, typename TClass, typename TInsertRet = typename TContainer<QString, TClass>::iterator>
	static bool registerMapContainerConverters(TInsertRet (TContainer<QString, TClass>::*insertMethod)(const QString &, const TClass &) = &TContainer<QString, TClass>::insert,
											   bool asMultiMap = false);
	//! Registers a custom type for list converisons
	template<typename T>
	static inline bool registerListConverters();
	template<typename T>
	static inline bool registerSetConverters();
	//! Registers a custom type for map converisons
	template<typename T>
	static inline bool registerMapConverters();
	//! Registers a custom type for list and map converisons
	template<typename T>
	static inline bool registerAllConverters();
	//! Registers a custom type for QSharedPointer and QPointer converisons
	template<typename T>
	static inline bool registerPointerConverters();
	//! Registers a custom type for QSharedPointer and QPointer converisons, including lists of those pointer types
	template<typename T>
	static inline bool registerPointerListConverters();
	//! Registers two types for pair conversion
	template<typename T, typename U>
	static inline bool registerPairConverters(const char *originalTypeName = nullptr);
	//! Registers a number of types for tuple conversion
	template<typename... TArgs>
	static inline bool registerTupleConverters(const char *originalTypeName = nullptr);

	//! @readAcFn{QJsonSerializer::allowDefaultNull}
	bool allowDefaultNull() const;
	//! @readAcFn{QJsonSerializer::keepObjectName}
	bool keepObjectName() const;
	//! @readAcFn{QJsonSerializer::enumAsString}
	bool enumAsString() const;
	//! @readAcFn{QJsonSerializer::validateBase64}
	bool validateBase64() const;
	//! @readAcFn{QJsonSerializer::useBcp47Locale}
	bool useBcp47Locale() const;
	//! @readAcFn{QJsonSerializer::validationFlags}
	ValidationFlags validationFlags() const;
	//! @readAcFn{QJsonSerializer::polymorphing}
	Polymorphing polymorphing() const;
	MultiMapMode multiMapMode() const;

	//! Serializers a QVariant value to a QJsonValue
	QJsonValue serialize(const QVariant &data) const;
	//! Serializers a QVariant value to a device
	void serializeTo(QIODevice *device, const QVariant &data) const; //MAJOR join as overload
	//! @copybrief QJsonSerializer::serializeTo(QIODevice *, const QVariant &) const
	void serializeTo(QIODevice *device, const QVariant &data, QJsonDocument::JsonFormat format) const;
	//! Serializers a QVariant value to a byte array
	QByteArray serializeTo(const QVariant &data) const; //MAJOR join as overload
	//! @copybrief QJsonSerializer::serializeTo(const QVariant &) const
	QByteArray serializeTo(const QVariant &data, QJsonDocument::JsonFormat format) const;

	//! Serializers a QObject, Q_GADGET or a list of one of those to json
	template <typename T>
	typename _qjsonserializer_helpertypes::json_type<T>::type serialize(const T &data) const;
	//! Serializers a QObject, Q_GADGET or a list of one of those to a device
	template <typename T>
	void serializeTo(QIODevice *device, const T &data, QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
	//! Serializers a QQObject, Q_GADGET or a list of one of those to a byte array
	template <typename T>
	QByteArray serializeTo(const T &data, QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;

	//! Deserializes a QJsonValue to a QVariant value, based on the given type id
	QVariant deserialize(const QJsonValue &json, int metaTypeId, QObject *parent = nullptr) const;
	//! Deserializes data from a device to a QVariant value, based on the given type id
	QVariant deserializeFrom(QIODevice *device, int metaTypeId, QObject *parent = nullptr) const;
	//! Deserializes data from a device to a QVariant value, based on the given type id
	QVariant deserializeFrom(const QByteArray &data, int metaTypeId, QObject *parent = nullptr) const;

	//! Deserializes a json to the given QObject type, Q_GADGET type or a list of one of those types
	template <typename T>
	T deserialize(const typename _qjsonserializer_helpertypes::json_type<T>::type &json, QObject *parent = nullptr) const;
	//! Deserializes data from a device to the given QObject type, Q_GADGET type or a list of one of those types
	template <typename T>
	T deserializeFrom(QIODevice *device, QObject *parent = nullptr) const;
	//! Deserializes data from a byte array to the given QObject type, Q_GADGET type or a list of one of those types
	template <typename T>
	T deserializeFrom(const QByteArray &data, QObject *parent = nullptr) const;

	//! Adds a custom type converter to this serializer
	template <typename T>
	void addJsonTypeConverter();
	//! @copybrief QJsonSerializer::addJsonTypeConverter()
	void addJsonTypeConverter(QSharedPointer<QJsonTypeConverter> converter);
	//! @private
	QT_DEPRECATED void addJsonTypeConverter(QJsonTypeConverter *converter);

public Q_SLOTS:
	//! @writeAcFn{QJsonSerializer::allowDefaultNull}
	void setAllowDefaultNull(bool allowDefaultNull);
	//! @writeAcFn{QJsonSerializer::keepObjectName}
	void setKeepObjectName(bool keepObjectName);
	//! @writeAcFn{QJsonSerializer::enumAsString}
	void setEnumAsString(bool enumAsString);
	//! @writeAcFn{QJsonSerializer::validateBase64}
	void setValidateBase64(bool validateBase64);
	//! @writeAcFn{QJsonSerializer::useBcp47Locale}
	void setUseBcp47Locale(bool useBcp47Locale);
	//! @writeAcFn{QJsonSerializer::validationFlags}
	void setValidationFlags(ValidationFlags validationFlags);
	//! @writeAcFn{QJsonSerializer::polymorphing}
	void setPolymorphing(Polymorphing polymorphing);
	void setMultiMapMode(MultiMapMode multiMapMode);

protected:
	//protected implementation -> internal use for the type converters
	QVariant getProperty(const char *name) const override;
	QJsonValue serializeSubtype(QMetaProperty property, const QVariant &value) const override;
	QVariant deserializeSubtype(QMetaProperty property, const QJsonValue &value, QObject *parent) const override;
	QJsonValue serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const override;
	QVariant deserializeSubtype(int propertyType, const QJsonValue &value, QObject *parent, const QByteArray &traceHint) const override;

private:
	friend class QJsonSerializerPrivate;
	QScopedPointer<QJsonSerializerPrivate> d;

	QJsonValue serializeVariant(int propertyType, const QVariant &value) const;
	QVariant deserializeVariant(int propertyType, const QJsonValue &value, QObject *parent) const;

	QJsonValue serializeValue(int propertyType, const QVariant &value) const;
	QVariant deserializeValue(int propertyType, const QJsonValue &value) const;

	QJsonValue serializeEnum(const QMetaEnum &metaEnum, const QVariant &value) const;
	QVariant deserializeEnum(const QMetaEnum &metaEnum, const QJsonValue &value) const;

	void writeToDevice(const QJsonValue &data, QIODevice *device, QJsonDocument::JsonFormat format) const;
	QJsonValue readFromDevice(QIODevice *device) const;

	QJsonValue serializeImpl(const QVariant &data) const;
	QT_DEPRECATED void serializeToImpl(QIODevice *device, const QVariant &data) const; //MAJOR remove
	void serializeToImpl(QIODevice *device, const QVariant &data, QJsonDocument::JsonFormat format) const;
	QT_DEPRECATED QByteArray serializeToImpl(const QVariant &data) const; //MAJOR remove
	QByteArray serializeToImpl(const QVariant &data, QJsonDocument::JsonFormat format) const;

	static void registerInverseTypedefImpl(int typeId, const char *normalizedTypeName);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QJsonSerializer::ValidationFlags)

//! A macro the mark a class as polymorphic
#define Q_JSON_POLYMORPHIC(x) \
	static_assert(std::is_same<decltype(x), bool>::value, "x must be bool"); \
	Q_CLASSINFO("polymorphic", #x)

// ------------- Generic Implementation -------------

template<typename T>
void QJsonSerializer::registerInverseTypedef(const char *typeName)
{
	qRegisterMetaType<T>(typeName);
	registerInverseTypedefImpl(qMetaTypeId<T>(), QMetaObject::normalizedType(typeName));
}

template <template<typename> class TContainer, typename TClass, typename TAppendRet>
bool QJsonSerializer::registerListContainerConverters(TAppendRet (TContainer<TClass>::*appendMethod)(const TClass &), void (TContainer<TClass>::*reserveMethod)(int))
{
	return QMetaType::registerConverter<TContainer<TClass>, QVariantList>([](const TContainer<TClass> &list) -> QVariantList {
		QVariantList l;
		l.reserve(list.size());
		for(const auto &v : list)
			l.append(QVariant::fromValue(v));
		return l;
	}) & QMetaType::registerConverter<QVariantList, TContainer<TClass>>([appendMethod, reserveMethod](const QVariantList &list) -> TContainer<TClass> {
		TContainer<TClass> l;
		if(reserveMethod)
			(l.*reserveMethod)(list.size());
		for(auto v : list) { // clazy:exclude=range-loop
			const auto vt = v.type();
			if(v.convert(qMetaTypeId<TClass>()))
				(l.*appendMethod)(v.value<TClass>());
			else {
				qWarning() << "Conversion to"
						   << QMetaType::typeName(qMetaTypeId<TContainer<TClass>>())
						   << "failed, could not convert element of type"
						   << QMetaType::typeName(vt);
				(l.*appendMethod)(TClass());
			}
		}
		return l;
	});
}

template<template <typename, typename> class TContainer, typename TClass, typename TInsertRet>
bool QJsonSerializer::registerMapContainerConverters(TInsertRet (TContainer<QString, TClass>::*insertMethod)(const QString &, const TClass &), bool asMultiMap)
{
	return QMetaType::registerConverter<TContainer<QString, TClass>, QVariantMap>([asMultiMap](const TContainer<QString, TClass> &map) -> QVariantMap {
		QVariantMap m;
		for(auto it = map.constBegin(); it != map.constEnd(); ++it) {
			if(asMultiMap)
				m.insertMulti(it.key(), QVariant::fromValue(it.value()));
			else
				m.insert(it.key(), QVariant::fromValue(it.value()));
		}
		return m;
	}) & QMetaType::registerConverter<QVariantMap, TContainer<QString, TClass>>([insertMethod](const QVariantMap &map) -> TContainer<QString, TClass> {
		TContainer<QString, TClass> m;
		for(auto it = map.constBegin(); it != map.constEnd(); ++it) {
			auto v = it.value();
			const auto vt = v.type();
			if(v.convert(qMetaTypeId<TClass>()))
				(m.*insertMethod)(it.key(), v.value<TClass>());
			else {
				qWarning() << "Conversion to"
						   << QMetaType::typeName(qMetaTypeId<TContainer<QString, TClass>>())
						   << "failed, could not convert element value of type"
						   << QMetaType::typeName(vt);
				(m.*insertMethod)(it.key(), TClass());
			}
		}
		return m;
	});
}

template<typename T>
bool QJsonSerializer::registerListConverters()
{
	return registerListContainerConverters<QList, T>() &
			registerListContainerConverters<QLinkedList, T>(&QLinkedList<T>::append, nullptr) &
			registerListContainerConverters<QVector, T>() &
			registerListContainerConverters<QStack, T>() &
			registerListContainerConverters<QQueue, T>();
}

template<typename T>
bool QJsonSerializer::registerSetConverters()
{
	return registerListContainerConverters<QSet, T>(&QSet<T>::insert);
}

template<typename T>
bool QJsonSerializer::registerMapConverters()
{
	return registerMapContainerConverters<QMap, T>() &
			registerMapContainerConverters<QHash, T>() &
			registerMapContainerConverters<QMultiMap, T>(&QMultiMap<QString, T>::insert, true) &
			registerMapContainerConverters<QMultiHash, T>(&QMultiHash<QString, T>::insert, true);
}

template<typename T>
bool QJsonSerializer::registerAllConverters()
{
	return registerListConverters<T>() &
			registerSetConverters<T>() &
			registerMapConverters<T>();
}

template<typename T>
bool QJsonSerializer::registerPointerConverters()
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject");
	return QMetaType::registerConverter<QSharedPointer<QObject>, QSharedPointer<T>>([](const QSharedPointer<QObject> &object) -> QSharedPointer<T> {
		return object.objectCast<T>();
	}) & QMetaType::registerConverter<QSharedPointer<T>, QSharedPointer<QObject>>([](const QSharedPointer<T> &object) -> QSharedPointer<QObject> {
		return object.template staticCast<QObject>();//must work, because of static assert
	}) & QMetaType::registerConverter<QPointer<QObject>, QPointer<T>>([](const QPointer<QObject> &object) -> QPointer<T> {
		return qobject_cast<T*>(object.data());
	}) & QMetaType::registerConverter<QPointer<T>, QPointer<QObject>>([](const QPointer<T> &object) -> QPointer<QObject> {
		return static_cast<QObject*>(object.data());
	});
}

template<typename T>
bool QJsonSerializer::registerPointerListConverters()
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject");
	return registerPointerConverters<T>() &
			registerAllConverters<QSharedPointer<T>>() &
			registerAllConverters<QPointer<T>>();
}

template<typename T1, typename T2>
bool QJsonSerializer::registerPairConverters(const char *originalTypeName)
{
	if(originalTypeName)
		registerInverseTypedef<std::pair<T1, T2>>(originalTypeName);
	return QMetaType::registerConverter<QPair<T1, T2>, QPair<QVariant, QVariant>>([](const QPair<T1, T2> &pair) -> QPair<QVariant, QVariant> {
		return {
			QVariant::fromValue(pair.first),
			QVariant::fromValue(pair.second)
		};
	}) & QMetaType::registerConverter<QPair<QVariant, QVariant>, QPair<T1, T2>>([](const QPair<QVariant, QVariant> &pair) -> QPair<T1, T2> {
		return {
			pair.first.value<T1>(),
			pair.second.value<T2>()
		};
	}) & QMetaType::registerConverter<std::pair<T1, T2>, QPair<QVariant, QVariant>>([](const std::pair<T1, T2> &pair) -> QPair<QVariant, QVariant> {
		return {
			QVariant::fromValue(pair.first),
			QVariant::fromValue(pair.second)
		};
	}) & QMetaType::registerConverter<QPair<QVariant, QVariant>, std::pair<T1, T2>>([](const QPair<QVariant, QVariant> &pair) -> std::pair<T1, T2> {
		return {
			pair.first.value<T1>(),
			pair.second.value<T2>()
		};
	});
}

template<typename... TArgs>
bool QJsonSerializer::registerTupleConverters(const char *originalTypeName)
{
	if(originalTypeName)
		registerInverseTypedef<std::tuple<TArgs...>>(originalTypeName);
	return QMetaType::registerConverter<std::tuple<TArgs...>, QVariantList>(&_qjsonserializer_helpertypes::tplToList<TArgs...>) &
			QMetaType::registerConverter<QVariantList, std::tuple<TArgs...>>(&_qjsonserializer_helpertypes::listToTpl<TArgs...>);
}

template<typename T>
typename _qjsonserializer_helpertypes::json_type<T>::type QJsonSerializer::serialize(const T &data) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be serialized");
	return _qjsonserializer_helpertypes::json_type<T>::convert(serializeImpl(_qjsonserializer_helpertypes::variant_helper<T>::toVariant(data)));
}

template<typename T>
void QJsonSerializer::serializeTo(QIODevice *device, const T &data, QJsonDocument::JsonFormat format) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be serialized");
	serializeToImpl(device, _qjsonserializer_helpertypes::variant_helper<T>::toVariant(data), format);
}

template<typename T>
QByteArray QJsonSerializer::serializeTo(const T &data, QJsonDocument::JsonFormat format) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be serialized");
	return serializeToImpl(_qjsonserializer_helpertypes::variant_helper<T>::toVariant(data), format);
}

template<typename T>
T QJsonSerializer::deserialize(const typename _qjsonserializer_helpertypes::json_type<T>::type &json, QObject *parent) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be deserialized");
	return _qjsonserializer_helpertypes::variant_helper<T>::fromVariant(deserialize(json, qMetaTypeId<T>(), parent));
}

template<typename T>
T QJsonSerializer::deserializeFrom(QIODevice *device, QObject *parent) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be deserialized");
	return _qjsonserializer_helpertypes::variant_helper<T>::fromVariant(deserializeFrom(device, qMetaTypeId<T>(), parent));
}

template<typename T>
T QJsonSerializer::deserializeFrom(const QByteArray &data, QObject *parent) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be deserialized");
	return _qjsonserializer_helpertypes::variant_helper<T>::fromVariant(deserializeFrom(data, qMetaTypeId<T>(), parent));
}

template<typename T>
void QJsonSerializer::addJsonTypeConverter()
{
	static_assert(std::is_base_of<QJsonTypeConverter, T>::value, "T must implement QJsonTypeConverter");
	addJsonTypeConverter(QSharedPointer<T>::create());
}

Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(QMultiMap)
Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(QMultiHash)

//! @file qjsonserializer.h The QJsonSerializer header file
#endif // QJSONSERIALIZER_H
