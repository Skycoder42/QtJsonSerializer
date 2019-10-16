#ifndef QJSONSERIALIZERBASE_P_H
#define QJSONSERIALIZERBASE_P_H

#include "qtjsonserializer_global.h"
#include "qjsonserializerbase.h"

#include <QtCore/QReadWriteLock>
#include <QtCore/QHash>

#include <QtCore/private/qobject_p.h>

class Q_JSONSERIALIZER_EXPORT QJsonSerializerBasePrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(QJsonSerializerBase)

public:
	using ValidationFlag = QJsonSerializerBase::ValidationFlag;
	using ValidationFlags = QJsonSerializerBase::ValidationFlags;
	using Polymorphing = QJsonSerializerBase::Polymorphing;
	using MultiMapMode = QJsonSerializerBase::MultiMapMode;

	template <typename TConverter>
	class ConverterCache {
	public:
		QSharedPointer<TConverter> get(int metaTypeId) const;
		void add(int metaTypeId, const QSharedPointer<TConverter> &converter);

		void clear();

	private:
		mutable QReadWriteLock lock {};
		QHash<int, QSharedPointer<TConverter>> cache;
	};

	template <typename TConverter>
	struct ConverterStore {
		mutable QReadWriteLock lock {};
		QList<QSharedPointer<TConverter>> store;

		ConverterStore() = default;
		ConverterStore(std::initializer_list<QSharedPointer<TConverter>> initData);

		void insertSorted(const QSharedPointer<TConverter> &converter);
	};

	static QByteArray getTypeName(int propertyType);

	static QReadWriteLock typedefLock;
	static QHash<int, QByteArray> typedefMapping;

	static ConverterStore<QJsonTypeConverterFactory> typeConverterFactories;

	bool allowNull = false;
	bool keepObjectName = false;
	bool enumAsString = false;
	bool useBcp47Locale = true;
	ValidationFlags validationFlags = ValidationFlag::StandardValidation;
	Polymorphing polymorphing = Polymorphing::Enabled;
	MultiMapMode multiMapMode = MultiMapMode::Map;
	bool ignoreStoredAttribute = false;

	mutable ConverterStore<QJsonTypeConverter> typeConverters;
	mutable ConverterCache<QJsonTypeConverter> serCache;
	mutable ConverterCache<QJsonTypeConverter> deserCache;

	template <typename TConverter>
	void insertSorted(const QSharedPointer<TConverter> &converter, QList<QSharedPointer<TConverter>> &list) const;

	QSharedPointer<QJsonTypeConverter> findSerConverter(int propertyType) const;
	QSharedPointer<QJsonTypeConverter> findDeserConverter(int &propertyType, QCborTag tag, QCborValue::Type type) const;

	QCborValue serializeValue(int propertyType, const QVariant &value) const;
	QVariant deserializeCborValue(int propertyType, const QCborValue &value) const;
	QVariant deserializeJsonValue(int propertyType, const QCborValue &value) const;
};

template<typename TConverter>
QSharedPointer<TConverter> QJsonSerializerBasePrivate::ConverterCache<TConverter>::get(int metaTypeId) const
{
	QReadLocker _{&lock};
	return cache.value(metaTypeId, nullptr);
}

template<typename TConverter>
void QJsonSerializerBasePrivate::ConverterCache<TConverter>::add(int metaTypeId, const QSharedPointer<TConverter> &converter)
{
	QWriteLocker _{&lock};
	cache.insert(metaTypeId, converter);
}

template<typename TConverter>
void QJsonSerializerBasePrivate::ConverterCache<TConverter>::clear()
{
	QWriteLocker _{&lock};
	cache.clear();
}

template<typename TConverter>
QJsonSerializerBasePrivate::ConverterStore<TConverter>::ConverterStore(std::initializer_list<QSharedPointer<TConverter>> initData)
	: store{std::move(initData)}
{
#ifndef QT_NO_DEBUG
	for (auto i = 1; i < store.size(); ++i)
		Q_ASSERT(store[i]->priority() <= store[i - 1]->priority());
#endif
}

template<typename TConverter>
void QJsonSerializerBasePrivate::ConverterStore<TConverter>::insertSorted(const QSharedPointer<TConverter> &converter)
{
	QWriteLocker _{&lock};
	for(auto it = store.begin(); it != store.end(); ++it) {
		if((*it)->priority() <= converter->priority()) {
			store.insert(it, converter);
			return;
		}
	}
	// not inserted -> add to end
	store.append(converter);
}

#endif // QJSONSERIALIZERBASE_P_H
