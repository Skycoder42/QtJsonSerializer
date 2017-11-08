#ifndef QJSONSERIALIZER_HELPERTYPES_H
#define QJSONSERIALIZER_HELPERTYPES_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qpointer.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <type_traits>

//! A collection of internally used type helpers
namespace _qjsonserializer_helpertypes {

//! test if a type can be serialized
template <typename T>
struct is_serializable : public std::integral_constant<bool, !std::is_pointer<T>::value> {}; //C++17 negation

//! @copydoc _qjsonserializer_helpertypes::is_serializable
template <typename T>
struct is_serializable<T*> : public std::is_base_of<QObject, T> {};

//! @copydoc _qjsonserializer_helpertypes::is_serializable
template <typename T>
struct is_serializable<QSharedPointer<T>> : public std::is_base_of<QObject, T> {};

//! @copydoc _qjsonserializer_helpertypes::is_serializable
template <typename T>
struct is_serializable<QPointer<T>> : public std::is_base_of<QObject, T> {};

//! @copydoc _qjsonserializer_helpertypes::is_serializable
template <typename T>
struct is_serializable<QList<T>> : public is_serializable<T> {};

//! @copydoc _qjsonserializer_helpertypes::is_serializable
template <typename T>
struct is_serializable<QMap<QString, T>> : public is_serializable<T> {};

//! @copydoc _qjsonserializer_helpertypes::is_serializable
template <typename T1, typename T2>
struct is_serializable<QPair<T1, T2>> : public std::conditional<is_serializable<T1>::value && is_serializable<T2>::value, std::true_type, std::false_type>::type {}; //C++17 conjunction



//! helper to get the gadget information
template <class T, class Enable = void>
struct gadget_helper
{
	//! Specify if T is a gadget
	static constexpr bool value = false;
	//! Convert a json value to the required type
	static inline QJsonValue convert(const QJsonValue &value) {
		return value;
	}
};

//! @copydoc _qjsonserializer_helpertypes::gadget_helper
template <class T>
struct gadget_helper<T, typename T::QtGadgetHelper>
{
	//! @copydoc _qjsonserializer_helpertypes::value
	static constexpr bool value = true;
	//! @copydoc _qjsonserializer_helpertypes::convert
	static inline QJsonObject convert(const QJsonValue &value) {
		return value.toObject();
	}
};

//! class to get the json type based on gadget or not
template <typename T>
struct json_type_raw :
		public std::conditional<gadget_helper<T>::value,
			QJsonObject,
			QJsonValue> {};

//! Get the json type for any basic type
template <typename T>
struct json_type : json_type_raw<T> {
	static_assert(is_serializable<T>::value, "Only QObject deriving classes can be serialized as pointer");

	//! Converts json value to json type
	static inline typename json_type_raw<T>::type convert(const QJsonValue &value) {
		return gadget_helper<T>::convert(value);
	}
};

//! @copydoc _qjsonserializer_helpertypes::json_type
template <typename T>
struct json_type<T*> {
	static_assert(is_serializable<T*>::value, "Only QObject deriving classes can be serialized as pointer");
	//! The json type
	typedef QJsonObject type;

	//! @copydoc _qjsonserializer_helpertypes::json_type::convert
	static inline type convert(const QJsonValue &value) {
		return value.toObject();
	}
};

//! @copydoc _qjsonserializer_helpertypes::json_type
template <typename T>
struct json_type<QSharedPointer<T>> {
	static_assert(is_serializable<QSharedPointer<T>>::value, "Only QObject deriving classes can be serialized as pointer");
	//! The json type
	typedef QJsonObject type;

	//! @copydoc _qjsonserializer_helpertypes::json_type::convert
	static inline type convert(const QJsonValue &value) {
		return value.toObject();
	}
};

//! @copydoc _qjsonserializer_helpertypes::json_type
template <typename T>
struct json_type<QPointer<T>> {
	static_assert(is_serializable<QPointer<T>>::value, "Only QObject deriving classes can be serialized as pointer");
	//! The json type
	typedef QJsonObject type;

	//! @copydoc _qjsonserializer_helpertypes::json_type::convert
	static inline type convert(const QJsonValue &value) {
		return value.toObject();
	}
};

//! @copydoc _qjsonserializer_helpertypes::json_type
template <typename T>
struct json_type<QList<T>> {
	static_assert(is_serializable<QList<T>>::value, "Only QObject deriving classes can be serialized as pointer");
	//! The json type
	typedef QJsonArray type;

	//! @copydoc _qjsonserializer_helpertypes::json_type::convert
	static inline type convert(const QJsonValue &value) {
		return value.toArray();
	}
};

//! @copydoc _qjsonserializer_helpertypes::json_type
template <typename T>
struct json_type<QMap<QString, T>> {
	static_assert(is_serializable<QMap<QString, T>>::value, "Only QObject deriving classes can be serialized as pointer");
	//! The json type
	typedef QJsonObject type;

	//! @copydoc _qjsonserializer_helpertypes::json_type::convert
	static inline type convert(const QJsonValue &value) {
		return value.toObject();
	}
};

//! @copydoc _qjsonserializer_helpertypes::json_type
template <typename T1, typename T2>
struct json_type<QPair<T1, T2>> {
	static_assert(is_serializable<QPair<T1, T2>>::value, "Only QObject deriving classes can be serialized as pointer");
	//! The json type
	typedef QJsonArray type;

	//! @copydoc _qjsonserializer_helpertypes::json_type::convert
	static inline type convert(const QJsonValue &value) {
		return value.toArray();
	}
};



//! helper to convert a type to variant
template <typename T>
struct variant_helper {
	//! Converts generic type to variant
	static inline QVariant toVariant(const T &data) {
		return QVariant::fromValue<T>(data);
	}
	//! Converts variant to generic type
	static inline T fromVariant(const QVariant &data) {
		return data.template value<T>();
	}
};

//! @copydoc _qjsonserializer_helpertypes::variant_helper
template <>
struct variant_helper<QVariant> {
	//! @copydoc _qjsonserializer_helpertypes::variant_helper::toVariant
	static inline QVariant toVariant(const QVariant &data) {
		return data;
	}
	//! @copydoc _qjsonserializer_helpertypes::variant_helper::fromVariant
	static inline QVariant fromVariant(const QVariant &data) {
		return data;
	}
};

}

#endif // QJSONSERIALIZER_HELPERTYPES_H
