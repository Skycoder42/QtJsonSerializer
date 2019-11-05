#ifndef QTJSONSERIALIZER_TYPEEXTRACTORS_H
#define QTJSONSERIALIZER_TYPEEXTRACTORS_H

#include "QtJsonSerializer/qjsontypeconverter.h"

#include <optional>
#include <tuple>
#include <variant>

#include <QtCore/qbytearray.h>
#include <QtCore/qpair.h>

namespace QtJsonSerializer::TypeExtractors {

template <template <typename> class TSmartPointer, typename TType>
class SmartPointerExtractor final : public TypeExtractor
{
public:
	using Pointer = TType*;
	using Type = TSmartPointer<TType>;

	QByteArray baseType() const final {
		return QByteArrayLiteral("pointer");
	}

	QList<int> subtypes() const final {
		return {qMetaTypeId<Pointer>()};
	}

	QVariant extract(const QVariant &value, int) const final {
		return QVariant::fromValue<Pointer>(value.value<Type>().get());
	}

	void emplace(QVariant &target, const QVariant &value, int) const final {
		target = QVariant::fromValue(Type{value.value<Pointer>()});
	}
};

template <typename TType>
class SmartPointerExtractor<QPointer, TType> final : public TypeExtractor
{
	static_assert(std::is_base_of_v<QObject, TType>, "QPointer can only handle QObject types");
public:
	using Pointer = TType*;
	using Type = QPointer<TType>;

	QByteArray baseType() const final {
		return QByteArrayLiteral("qpointer");
	}

	QList<int> subtypes() const final {
		return {qMetaTypeId<Pointer>()};
	}

	QVariant extract(const QVariant &value, int) const final {
		return QVariant::fromValue<Pointer>(value.value<Type>().data());
	}

	void emplace(QVariant &target, const QVariant &value, int) const final {
		target = QVariant::fromValue(Type{value.value<Pointer>()});
	}
};

template <template <typename, typename> class TPair, typename TFirst, typename TSecond>
class PairExtractor final : public TypeExtractor
{
public:
	using Type = TPair<TFirst, TSecond>;

	QByteArray baseType() const final {
		return QByteArrayLiteral("pair");
	}

	QList<int> subtypes() const final {
		return {qMetaTypeId<TFirst>(), qMetaTypeId<TSecond>()};
	}

	QVariant extract(const QVariant &value, int index) const final {
		const auto vPair = value.value<Type>();
		switch (index) {
		case 0:
			return QVariant::fromValue(vPair.first);
		case 1:
			return QVariant::fromValue(vPair.second);
		default:
			return {};
		}
	}

	void emplace(QVariant &target, const QVariant &value, int index) const final {
		Q_ASSERT(target.userType() == qMetaTypeId<Type>());
		const auto vPair = reinterpret_cast<Type*>(target.data());
		switch (index) {
		case 0:
			vPair->first = value.value<TFirst>();
			break;
		case 1:
			vPair->second = value.value<TSecond>();
			break;
		default:
			break;
		}
	}
};

template <typename TValue>
class OptionalExtractor final : public TypeExtractor
{
public:
	using Type = std::optional<TValue>;

	QByteArray baseType() const final {
		return QByteArrayLiteral("optional");
	}

	QList<int> subtypes() const final {
		return {qMetaTypeId<TValue>()};
	}

	QVariant extract(const QVariant &value, int) const final {
		if (const auto opt = value.value<std::optional<TValue>>(); opt)
			return QVariant::fromValue(*opt);
		else
			return QVariant::fromValue(nullptr);
	}

	void emplace(QVariant &target, const QVariant &value, int) const final {
		if (value.isNull())
			target = QVariant::fromValue<std::optional<TValue>>(std::nullopt);
		else
			target = QVariant::fromValue<std::optional<TValue>>(value.value<TValue>());
	}
};

template <typename... TValues>
class TupleExtractor final : public TypeExtractor
{
public:
	using Type = std::tuple<TValues...>;

private:
	template <size_t Index>
	inline QVariant getIf(const Type &, size_t) const {
		return {};
	}
	template <size_t Index, typename TValue, typename... TRest>
	inline QVariant getIf(const Type &tpl, size_t index) const {
		if (Index == index)
			return QVariant::fromValue(std::get<Index>(tpl));
		else
			return getIf<Index + 1, TRest...>(tpl, index);
	}

	template <size_t Index>
	inline void setIf(Type *, size_t, const QVariant &) const {}
	template <size_t Index, typename TValue, typename... TRest>
	inline void setIf(Type *tpl, size_t index, const QVariant &value) const {
		if (Index == index)
			std::get<Index>(*tpl) = value.value<TValue>();
		else
			setIf<Index + 1, TRest...>(tpl, index, value);
	}

public:
	QByteArray baseType() const final {
		return QByteArrayLiteral("tuple");
	}

	QList<int> subtypes() const final {
		return {qMetaTypeId<TValues>()...};
	}

	QVariant extract(const QVariant &value, int index) const final {
		return getIf<0, TValues...>(value.value<Type>(), static_cast<size_t>(index));
	}

	void emplace(QVariant &target, const QVariant &value, int index) const final {
		Q_ASSERT(target.userType() == qMetaTypeId<Type>());
		setIf<0, TValues...>(reinterpret_cast<Type*>(target.data()), static_cast<size_t>(index), value);
	}
};

template <typename... TValues>
class VariantExtractor final : public TypeExtractor
{
public:
	using Type = std::variant<TValues...>;

private:
	template <typename _>
	inline Type constructIfType(const QVariant &) const { return {}; }
	template <typename _, typename TValue, typename... TArgs>
	inline Type constructIfType(const QVariant &var) const {
		if (var.userType() == qMetaTypeId<TValue>())
			return var.value<TValue>();
		else
			return constructIfType<_, TArgs...>(var);
	}

public:
	QByteArray baseType() const final {
		return QByteArrayLiteral("variant");
	}

	QList<int> subtypes() const final {
		return {qMetaTypeId<TValues>()...};
	}

	QVariant extract(const QVariant &value, int) const final {
		return std::visit([](const auto &x) {
			return QVariant::fromValue(x);
		}, value.value<Type>());
	}

	void emplace(QVariant &target, const QVariant &value, int) const final {
		target = QVariant::fromValue(constructIfType<void, TValues...>(value));
	}
};

}

#endif // QTJSONSERIALIZER_TYPEEXTRACTORS_H
