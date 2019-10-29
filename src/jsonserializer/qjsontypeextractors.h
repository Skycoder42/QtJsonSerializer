#ifndef QJSONTYPEEXTRACTORS_H
#define QJSONTYPEEXTRACTORS_H

#include "QtJsonSerializer/qjsontypeconverter.h"

#include <optional>
#include <tuple>
#include <variant>

#include <QtCore/qbytearray.h>
#include <QtCore/qpair.h>

namespace QJsonTypeExtractors {

template <template <typename, typename> class TPair, typename TFirst, typename TSecond>
class PairExtractor : public QJsonTypeExtractor
{
public:
	QByteArray baseType() const override {
		return QByteArrayLiteral("pair");
	}

	QList<int> subtypes() const override {
		return {qMetaTypeId<TFirst>(), qMetaTypeId<TSecond>()};
	}

	QVariant extract(const QVariant &value, int index) const override {
		const auto vPair = value.value<TPair<TFirst, TSecond>>();
		switch (index) {
		case 0:
			return QVariant::fromValue(vPair.first);
		case 1:
			return QVariant::fromValue(vPair.second);
		default:
			return {};
		}
	}

	void emplace(QVariant &target, int index, const QVariant &value) const override {
		using TFull = TPair<TFirst, TSecond>;
		Q_ASSERT(target.userType() == qMetaTypeId<TFull>());
		const auto vPair = reinterpret_cast<TFull*>(target.data());
		switch (index) {
		case 0:
			vPair.first = value.value<TFirst>();
			break;
		case 1:
			vPair.second = value.value<TSecond>();
			break;
		default:
			break;
		}
	}
};

template <typename TValue>
class OptionalExtractor : public QJsonTypeExtractor
{
public:
	QByteArray baseType() const override {
		return QByteArrayLiteral("optional");
	}

	QList<int> subtypes() const override {
		return {qMetaTypeId<TValue>()};
	}

	QVariant extract(const QVariant &value, int) const override {
		if (const auto opt = value.value<std::optional<TValue>>(); opt)
			return QVariant::fromValue(*opt);
		else
			return QVariant::fromValue(nullptr);
	}

	void emplace(QVariant &target, int, const QVariant &value) const override {
		if (value.isNull())
			target = QVariant::fromValue<std::optional<TValue>>(std::nullopt);
		else
			target = QVariant::fromValue<std::optional<TValue>>(value.value<TValue>());
	}
};

template <typename... TValues>
class TupleExtractor : public QJsonTypeExtractor
{
private:
	template <size_t Index>
	inline QVariant getIf(const std::tuple<TValues...> &, size_t) const {
		return {};
	}
	template <size_t Index, typename TValue, typename... TRest>
	inline QVariant getIf(const std::tuple<TValues...> &tpl, size_t index) const {
		if (Index == index)
			return QVariant::fromValue(std::get<Index>(tpl));
		else
			return getIf<Index + 1, TRest...>(tpl, index);
	}

	template <size_t Index>
	inline void setIf(std::tuple<TValues...> *, size_t, const QVariant &) const {}
	template <size_t Index, typename TValue, typename... TRest>
	inline void setIf(std::tuple<TValues...> *tpl, size_t index, const QVariant &value) const {
		if (Index == index)
			std::get<Index>(*tpl) = value.value<TValue>();
		else
			setIf<Index + 1, TRest...>(tpl, index, value);
	}

public:
	QByteArray baseType() const override {
		return QByteArrayLiteral("tuple");
	}

	QList<int> subtypes() const override {
		return {qMetaTypeId<TValues>()...};
	}

	QVariant extract(const QVariant &value, int index) const override {
		return getIf<0, TValues...>(value.value<std::tuple<TValues...>>(), static_cast<size_t>(index));
	}

	void emplace(QVariant &target, int index, const QVariant &value) const override {
		using TFull = std::tuple<TValues...>;
		Q_ASSERT(target.userType() == qMetaTypeId<TFull>());
		setIf<0, TValues...>(reinterpret_cast<TFull*>(target.data()), static_cast<size_t>(index), value);
	}
};

template <typename... TValues>
class VariantExtractor : public QJsonTypeExtractor
{
private:
	template <int _>
	inline std::variant<TValues...> constructIfType(const QVariant &) { return {}; }
	template <int _, typename TValue, typename... TArgs>
	inline std::variant<TValues...> constructIfType(const QVariant &var) {
		if (var.userType() == qMetaTypeId<TValue>())
			return var.value<TValue>();
		else
			return constructIfType<_, TArgs...>(var);
	}

public:
	QByteArray baseType() const override {
		return QByteArrayLiteral("variant");
	}

	QList<int> subtypes() const override {
		return {qMetaTypeId<TValues>()...};
	}

	QVariant extract(const QVariant &value, int) const override {
		return std::visit([](const auto &x) {
			return QVariant::fromValue(x);
		}, value.value<std::variant<TValues...>>());
	}

	void emplace(QVariant &target, int, const QVariant &value) const override {
		target = QVariant::fromValue(constructIfType<0, TValues...>(value));
	}
};

}

#endif // QJSONTYPEEXTRACTORS_H
