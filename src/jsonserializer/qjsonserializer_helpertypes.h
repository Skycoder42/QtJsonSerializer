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
#include <tuple>

namespace _qjsonserializer_helpertypes {

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
struct is_serializable : public std::integral_constant<bool, !std::is_pointer<T>::value> {}; //C++17 negation

template <typename T>
struct is_serializable<T*> : public std::conditional<std::is_base_of<QObject, T>::value || gadget_helper<T>::value, std::true_type, std::false_type>::type {}; //C++17 disjunction

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

template <typename T1, typename T2>
struct is_serializable<std::pair<T1, T2>> : public std::conditional<is_serializable<T1>::value && is_serializable<T2>::value, std::true_type, std::false_type>::type {}; //C++17 conjunction

template <typename T>
struct is_serializable<std::tuple<T>> : public is_serializable<T> {};

template <typename T1, typename T2, typename... TArgs>
struct is_serializable<std::tuple<T1, T2, TArgs...>> : public std::conditional<is_serializable<T1>::value && is_serializable<std::tuple<T2, TArgs...>>::value, std::true_type, std::false_type>::type {}; //C++17 conjunction



template <typename T>
struct json_type_raw :
		public std::conditional<gadget_helper<T>::value,
			QJsonObject,
			QJsonValue> {};

template <typename T>
struct json_type : json_type_raw<T> {
	static_assert(is_serializable<T>::value, "Only QObject deriving classes or gadget pointers can be serialized as pointer");

	static inline typename json_type_raw<T>::type convert(const QJsonValue &jsonValue) {
		return gadget_helper<T>::convert(jsonValue);
	}
};

template <typename T>
struct json_type<T*> {
	static_assert(is_serializable<T*>::value, "Only QObject deriving classes or gadget pointers can be serialized as pointer");
	using type = QJsonObject;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename T>
struct json_type<QSharedPointer<T>> {
	static_assert(is_serializable<QSharedPointer<T>>::value, "Only QObject deriving classes can be serialized as shared pointer");
	using type = QJsonObject;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename T>
struct json_type<QPointer<T>> {
	static_assert(is_serializable<QPointer<T>>::value, "Only QObject deriving classes can be serialized as QPointer");
	using type = QJsonObject;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename T>
struct json_type<QList<T>> {
	static_assert(is_serializable<QList<T>>::value, "The value type of a QList must be serializable for it to also be serializable");
	using type = QJsonArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};

template <typename T>
struct json_type<QMap<QString, T>> {
	static_assert(is_serializable<QMap<QString, T>>::value, "The value type of a QMap must be serializable for it to also be serializable");
	using type = QJsonObject;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toObject();
	}
};

template <typename T1, typename T2>
struct json_type<QPair<T1, T2>> {
	static_assert(is_serializable<QPair<T1, T2>>::value, "All elements of a QPair must be serializable for it to also be serializable");
	using type = QJsonArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};

template <typename T1, typename T2>
struct json_type<std::pair<T1, T2>> {
	static_assert(is_serializable<std::pair<T1, T2>>::value, "All elements of a std::pair must be serializable for it to also be serializable");
	using type = QJsonArray;

	static inline type convert(const QJsonValue &jsonValue) {
		return jsonValue.toArray();
	}
};

template <typename... TArgs>
struct json_type<std::tuple<TArgs...>> {
	static_assert(is_serializable<std::tuple<TArgs...>>::value, "All elements of a std::tuple must be serializable for it to also be serializable");
	using type = QJsonArray;

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



namespace tuple_helpers {

template<size_t... Is>
struct seq { };

template<size_t N, size_t... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

template<size_t... Is>
struct gen_seq<0, Is...> : seq<Is...> { };



template<typename TTuple, size_t... Is>
void for_each_to_list(TTuple&& tpl, QVariantList &list, seq<Is...>)
{
	auto l = { (list.append(QVariant::fromValue(std::get<Is>(tpl))), 0)... };
	Q_UNUSED(l);
}

template<typename TTuple, size_t... Is>
void for_each_from_list(TTuple&& tpl, const QVariantList &list, seq<Is...>)
{
	auto l = { (std::get<Is>(tpl) = list[Is].value<typename std::tuple_element<Is, typename std::decay<TTuple>::type>::type>(), 0)... };
	Q_UNUSED(l);
}

}

template <typename... TArgs>
QVariantList tplToList(const std::tuple<TArgs...> &tpl) {
	QVariantList list;
	list.reserve(sizeof...(TArgs));
	tuple_helpers::for_each_to_list(tpl, list, tuple_helpers::gen_seq<sizeof...(TArgs)>{});
	return list;
}

template <typename... TArgs>
std::tuple<TArgs...> listToTpl(const QVariantList &list) {
	std::tuple<TArgs...> tpl;
	tuple_helpers::for_each_from_list(tpl, list, tuple_helpers::gen_seq<sizeof...(TArgs)>{});
	return tpl;
}

}

#endif // QJSONSERIALIZER_HELPERTYPES_H
