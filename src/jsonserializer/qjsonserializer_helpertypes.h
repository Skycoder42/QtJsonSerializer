#ifndef QJSONSERIALIZER_HELPERTYPES_H
#define QJSONSERIALIZER_HELPERTYPES_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <type_traits>

//! A collection of internally used type helpers
namespace _qjsonserializer_helpertypes {

//! Tests if a given type has a meta object
template <typename T>
struct has_metaobject : public std::is_void<typename T::QtGadgetHelper> {};
//! @copydoc _qjsonserializer_helpertypes::has_metaobject
template <typename T>
struct has_metaobject<T*> : public std::is_base_of<QObject, T> {};
//! @copydoc _qjsonserializer_helpertypes::has_metaobject
template <>
struct has_metaobject<QVariant> : public std::true_type {};

//! The main type helper class
template <typename T>
struct type_helper : public has_metaobject<T> {
	//! The template type
	typedef T Type;
	//! The object type
	typedef T ObjectType;
	//! The matching json type
	typedef QJsonObject JsonType;

	//! Converts json value to json type
	static inline JsonType convert(const QJsonValue &value) {
		return value.toObject();
	}
	//! Converts generic type to variant
	static inline QVariant variant(const Type &data) {
		return QVariant::fromValue(data);
	}
};

//! @copydoc _qjsonserializer_helpertypes::type_helper
template <typename T>
struct type_helper<QList<T>> : public has_metaobject<T> {
	//! @copydoc _qjsonserializer_helpertypes::type_helper::Type
	typedef QList<T> Type;
	//! @copydoc _qjsonserializer_helpertypes::type_helper::ObjectType
	typedef T ObjectType;
	//! @copydoc _qjsonserializer_helpertypes::type_helper::JsonType
	typedef QJsonArray JsonType;

	//! @copydoc _qjsonserializer_helpertypes::type_helper::convert
	static inline JsonType convert(const QJsonValue &value) {
		return value.toArray();
	}
	//! @copydoc _qjsonserializer_helpertypes::type_helper::variant
	static inline QVariant variant(const Type &data) {
		return QVariant::fromValue(data);
	}
};

//! @copydoc _qjsonserializer_helpertypes::type_helper
template <>
struct type_helper<QVariant> : public has_metaobject<QVariant> {
	//! @copydoc _qjsonserializer_helpertypes::type_helper::Type
	typedef QVariant Type;
	//! @copydoc _qjsonserializer_helpertypes::type_helper::ObjectType
	typedef QVariant ObjectType;
	//! @copydoc _qjsonserializer_helpertypes::type_helper::JsonType
	typedef QJsonValue JsonType;

	//! @copydoc _qjsonserializer_helpertypes::type_helper::convert
	static inline JsonType convert(const QJsonValue &value) {
		return value;
	}
	//! @copydoc _qjsonserializer_helpertypes::type_helper::variant
	static inline QVariant variant(const Type &data) {
		return data;
	}
};



}

#endif // QJSONSERIALIZER_HELPERTYPES_H
