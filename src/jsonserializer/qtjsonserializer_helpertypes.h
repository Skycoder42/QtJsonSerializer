#ifndef QTJSONSERIALIZER_HELPERTYPES_H
#define QTJSONSERIALIZER_HELPERTYPES_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qvector.h>

#include <QtCore/qstack.h>
#include <QtCore/qqueue.h>
#include <QtCore/qset.h>
#include <QtCore/qmap.h>
#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qpointer.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qcborvalue.h>
#include <QtCore/qcbormap.h>
#include <QtCore/qcborarray.h>

#include <type_traits>
#include <tuple>
#include <optional>
#include <variant>

#define QT_NO_LINKED_LIST
namespace QtJsonSerializer::__private {

template <class T, class Enable = void>
struct gadget_helper
{
	static constexpr bool value = false;
	static inline QJsonValue convert(const QJsonValue &jsonValue) {
		return jsonValue;
	}
};

template <class T>
struct gadget_helper<T, typename T::QtGadgetHelper>
{
	static constexpr bool value = true;
	static inline QJsonObject convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};



template <typename T>
struct is_serializable : public std::negation<std::is_pointer<T>> {};

template <typename T>
struct is_serializable<T*> : public std::disjunction<std::is_base_of<QObject, T>, gadget_helper<T>> {};

template <typename T>
struct is_serializable<QSharedPointer<T>> : public std::is_base_of<QObject, T> {};

template <typename T>
struct is_serializable<QPointer<T>> : public std::is_base_of<QObject, T> {};

template <typename T>
struct is_serializable<QList<T>> : public is_serializable<T> {};

#ifndef QT_NO_LINKED_LIST
template <typename T>
struct is_serializable<QLinkedList<T>> : public is_serializable<T> {};
#endif

template <typename T>
struct is_serializable<QStack<T>> : public is_serializable<T> {};

template <typename T>
struct is_serializable<QQueue<T>> : public is_serializable<T> {};

template <typename TKey, typename TValue>
struct is_serializable<QMap<TKey, TValue>> : public std::conjunction<is_serializable<TKey>, is_serializable<TValue>> {};

template <typename TKey, typename TValue>
struct is_serializable<QMultiMap<TKey, TValue>> : public std::conjunction<is_serializable<TKey>, is_serializable<TValue>> {};

template <typename TKey, typename TValue>
struct is_serializable<QHash<TKey, TValue>> : public std::conjunction<is_serializable<TKey>, is_serializable<TValue>> {};

template <typename TKey, typename TValue>
struct is_serializable<QMultiHash<TKey, TValue>> : public std::conjunction<is_serializable<TKey>, is_serializable<TValue>> {};

template <typename T1, typename T2>
struct is_serializable<QPair<T1, T2>> : public std::conjunction<is_serializable<T1>, is_serializable<T2>> {};

template <typename... TArgs>
struct is_serializable<std::tuple<TArgs...>> : public std::conjunction<is_serializable<TArgs>...> {};

template <typename T>
struct is_serializable<std::optional<T>> : public is_serializable<T> {};

template <typename... TArgs>
struct is_serializable<std::variant<TArgs...>> : public std::conjunction<is_serializable<TArgs>...> {};



template <typename T>
struct json_type_raw : public std::conditional<gadget_helper<T>::value, QJsonObject, QJsonValue> {};

template <typename T>
struct json_type : public json_type_raw<T> {
	static_assert(is_serializable<T>::value, "Type T must be serializable to be used in a generic expression");

	static inline typename json_type_raw<T>::type convert(const QJsonValue &jsonValue) {
		return gadget_helper<T>::convert(jsonValue);
	}
};

template <typename T>
struct json_type<T*> {
	static_assert(is_serializable<T*>::value, "Only QObject deriving classes or gadget pointers can be serialized as pointer");
	using type = QJsonObject;
	using cborType = QCborMap;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename T>
struct json_type<QSharedPointer<T>> {
	static_assert(is_serializable<QSharedPointer<T>>::value, "Only QObject deriving classes can be serialized as shared pointer");
	using type = QJsonObject;
	using cborType = QCborMap;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename T>
struct json_type<QPointer<T>> {
	static_assert(is_serializable<QPointer<T>>::value, "Only QObject deriving classes can be serialized as QPointer");
	using type = QJsonObject;
	using cborType = QCborMap;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename T>
struct json_type<QList<T>> {
	static_assert(is_serializable<QList<T>>::value, "The value type of a QList must be serializable for it to also be serializable");
	using type = QJsonArray;
	using cborType = QCborArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};



#ifndef QT_NO_LINKED_LIST
template <typename T>
struct json_type<QLinkedList<T>> {
	static_assert(is_serializable<QLinkedList<T>>::value, "The value type of a QLinkedList must be serializable for it to also be serializable");
	using type = QJsonArray;
	using cborType = QCborArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};
#endif

template <typename T>
struct json_type<QStack<T>> {
	static_assert(is_serializable<QStack<T>>::value, "The value type of a QStack must be serializable for it to also be serializable");
	using type = QJsonArray;
	using cborType = QCborArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};

template <typename T>
struct json_type<QQueue<T>> {
	static_assert(is_serializable<QQueue<T>>::value, "The value type of a QQueue must be serializable for it to also be serializable");
	using type = QJsonArray;
	using cborType = QCborArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};

template <typename TKey, typename TValue>
struct json_type<QMap<TKey, TValue>> {
	static_assert(is_serializable<QMap<TKey, TValue>>::value, "The key and value types of a QMap must be serializable for it to also be serializable");
	using type = QJsonObject;
	using cborType = QCborMap;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename TKey, typename TValue>
struct json_type<QMultiMap<TKey, TValue>> {
	static_assert(is_serializable<QMultiMap<TKey, TValue>>::value, "The key and value types of a QMultiMap must be serializable for it to also be serializable");
	using type = QJsonObject;
	using cborType = QCborMap;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename TKey, typename TValue>
struct json_type<QHash<TKey, TValue>> {
	static_assert(is_serializable<QHash<TKey, TValue>>::value, "The key and value types of a QHash must be serializable for it to also be serializable");
	using type = QJsonObject;
	using cborType = QCborMap;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename TKey, typename TValue>
struct json_type<QMultiHash<TKey, TValue>> {
	static_assert(is_serializable<QMultiHash<TKey, TValue>>::value, "The key and value types of a QMultiHash must be serializable for it to also be serializable");
	using type = QJsonObject;
	using cborType = QCborMap;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename T1, typename T2>
struct json_type<QPair<T1, T2>> {
	static_assert(is_serializable<QPair<T1, T2>>::value, "All elements of a QPair must be serializable for it to also be serializable");
	using type = QJsonArray;
	using cborType = QCborArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};


template <typename... TArgs>
struct json_type<std::tuple<TArgs...>> {
	static_assert(is_serializable<std::tuple<TArgs...>>::value, "All elements of a std::tuple must be serializable for it to also be serializable");
	using type = QJsonArray;
	using cborType = QCborArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};

template <typename... TArgs>
struct json_type<std::variant<TArgs...>> {
	static_assert(is_serializable<std::variant<TArgs...>>::value, "All elements of a std::variant must be serializable for it to also be serializable");
	using type = QJsonArray;
	using cborType = QCborArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
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

#endif // QTJSONSERIALIZER_HELPERTYPES_H
