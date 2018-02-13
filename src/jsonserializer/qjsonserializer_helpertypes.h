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

namespace _qjsonserializer_helpertypes {

template <typename T>
struct is_serializable : public std::integral_constant<bool, !std::is_pointer<T>::value> {}; //C++17 negation

template <typename T>
struct is_serializable<T*> : public std::is_base_of<QObject, T> {};

template <typename T>
struct is_serializable<QSharedPointer<T>> : public std::is_base_of<QObject, T> {};

template <typename T>
struct is_serializable<QPointer<T>> : public std::is_base_of<QObject, T> {};

template <typename T>
struct is_serializable<QList<T>> : public is_serializable<T> {};

template <typename T>
struct is_serializable<QMap<QString, T>> : public is_serializable<T> {};

template <typename T1, typename T2>
struct is_serializable<QPair<T1, T2>> : public std::conditional<is_serializable<T1>::value && is_serializable<T2>::value, std::true_type, std::false_type>::type {}; //C++17 conjunction



template <class T, class Enable = void>
struct gadget_helper
{
	static constexpr bool value = false;
	static inline QJsonValue convert(const QJsonValue &value) {
		return value;
	}
};

template <class T>
struct gadget_helper<T, typename T::QtGadgetHelper>
{
	static constexpr bool value = true;
	static inline QJsonObject convert(const QJsonValue &value) {
		return value.toObject();
	}
};

template <typename T>
struct json_type_raw :
		public std::conditional<gadget_helper<T>::value,
			QJsonObject,
			QJsonValue> {};

template <typename T>
struct json_type : json_type_raw<T> {
	static_assert(is_serializable<T>::value, "Only QObject deriving classes can be serialized as pointer");

	static inline typename json_type_raw<T>::type convert(const QJsonValue &value) {
		return gadget_helper<T>::convert(value);
	}
};

template <typename T>
struct json_type<T*> {
	static_assert(is_serializable<T*>::value, "Only QObject deriving classes can be serialized as pointer");
	typedef QJsonObject type;

	static inline type convert(const QJsonValue &value) {
		return value.toObject();
	}
};

template <typename T>
struct json_type<QSharedPointer<T>> {
	static_assert(is_serializable<QSharedPointer<T>>::value, "Only QObject deriving classes can be serialized as pointer");
	typedef QJsonObject type;

	static inline type convert(const QJsonValue &value) {
		return value.toObject();
	}
};

template <typename T>
struct json_type<QPointer<T>> {
	static_assert(is_serializable<QPointer<T>>::value, "Only QObject deriving classes can be serialized as pointer");
	typedef QJsonObject type;

	static inline type convert(const QJsonValue &value) {
		return value.toObject();
	}
};

template <typename T>
struct json_type<QList<T>> {
	static_assert(is_serializable<QList<T>>::value, "Only QObject deriving classes can be serialized as pointer");
	typedef QJsonArray type;

	static inline type convert(const QJsonValue &value) {
		return value.toArray();
	}
};

template <typename T>
struct json_type<QMap<QString, T>> {
	static_assert(is_serializable<QMap<QString, T>>::value, "Only QObject deriving classes can be serialized as pointer");
	typedef QJsonObject type;

	static inline type convert(const QJsonValue &value) {
		return value.toObject();
	}
};

template <typename T1, typename T2>
struct json_type<QPair<T1, T2>> {
	static_assert(is_serializable<QPair<T1, T2>>::value, "Only QObject deriving classes can be serialized as pointer");
	typedef QJsonArray type;

	static inline type convert(const QJsonValue &value) {
		return value.toArray();
	}
};



template <typename T>
struct variant_helper {
	static inline QVariant toVariant(const T &data) {
		return QVariant::fromValue<T>(data);
	}
	static inline T fromVariant(const QVariant &data) {
		return data.template value<T>();
	}
};

template <>
struct variant_helper<QVariant> {
	static inline QVariant toVariant(const QVariant &data) {
		return data;
	}
	static inline QVariant fromVariant(const QVariant &data) {
		return data;
	}
};

}

#endif // QJSONSERIALIZER_HELPERTYPES_H
