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

namespace _qjsonserializer_helpertypes {

template <typename T>
struct has_metaobject : public std::is_void<typename T::QtGadgetHelper> {};
template <typename T>
struct has_metaobject<T*> : public std::is_base_of<QObject, T> {};
template <>
struct has_metaobject<QVariant> : public std::true_type {};

struct object_tag {};
struct list_tag {};
struct variant_tag {};

template <typename T>
struct type_helper : public has_metaobject<T> {
	typedef T Type;
	typedef object_tag Tag;
	typedef T ObjectType;
	typedef QJsonObject JsonType;

	static inline JsonType convert(const QJsonValue &value) {
		return value.toObject();
	}
	static inline QVariant variant(const Type &data) {
		return QVariant::fromValue(data);
	}
};

template <typename T>
struct type_helper<QList<T>> : public has_metaobject<T> {
	typedef QList<T> Type;
	typedef list_tag Tag;
	typedef T ObjectType;
	typedef QJsonArray JsonType;

	static inline JsonType convert(const QJsonValue &value) {
		return value.toArray();
	}
	static inline QVariant variant(const Type &data) {
		return QVariant::fromValue(data);
	}
};

template <>
struct type_helper<QVariant> : public has_metaobject<QVariant> {
	typedef QVariant Type;
	typedef variant_tag Tag;
	typedef QVariant ObjectType;
	typedef QJsonValue JsonType;

	static inline JsonType convert(const QJsonValue &value) {
		return value;
	}
	static inline QVariant variant(const Type &data) {
		return data;
	}
};



}

#endif // QJSONSERIALIZER_HELPERTYPES_H
