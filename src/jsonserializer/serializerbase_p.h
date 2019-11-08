#ifndef QTJSONSERIALIZER_SERIALIZERBASE_P_H
#define QTJSONSERIALIZER_SERIALIZERBASE_P_H

#include "qtjsonserializer_global.h"
#include "serializerbase.h"

#include <QtCore/QReadWriteLock>
#include <QtCore/QHash>
#include <QtCore/QLoggingCategory>

#include <QtCore/private/qobject_p.h>

namespace QtJsonSerializer {

class Q_JSONSERIALIZER_EXPORT SerializerBasePrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(SerializerBase)

public:
	using ValidationFlag = SerializerBase::ValidationFlag;
	using ValidationFlags = SerializerBase::ValidationFlags;
	using Polymorphing = SerializerBase::Polymorphing;
	using MultiMapMode = SerializerBase::MultiMapMode;

	template <typename TConverter>
	class ThreadSafeStore {
	public:
		ThreadSafeStore() = default;
		ThreadSafeStore(std::initializer_list<std::pair<int, QSharedPointer<TConverter>>> initData);

		QSharedPointer<TConverter> get(int metaTypeId) const;
		void add(int metaTypeId, const QSharedPointer<TConverter> &converter);

		void clear();

	private:
		mutable QReadWriteLock _lock {};
		QHash<int, QSharedPointer<TConverter>> _store;
	};

	template <typename TConverter>
	struct ConverterStore {
		mutable QReadWriteLock lock {};
		QList<QSharedPointer<TConverter>> store;
		QAtomicInt factoryOffset = 0;

		ConverterStore() = default;
		ConverterStore(std::initializer_list<QSharedPointer<TConverter>> initData);

		void insertSorted(const QSharedPointer<TConverter> &converter);
		void insertSorted(const QSharedPointer<TConverter> &converter, QWriteLocker &locker);
	};

	static ThreadSafeStore<TypeExtractor> extractors;

	static QReadWriteLock typeConverterFactoryLock;
	static QList<TypeConverterFactory*> typeConverterFactories;

	bool allowNull = false;
	bool keepObjectName = false;
	bool enumAsString = false;
	bool versionAsString = false;
	bool dateAsTimeStamp = false;
	bool useBcp47Locale = true;
	ValidationFlags validationFlags = ValidationFlag::StandardValidation;
	Polymorphing polymorphing = Polymorphing::Enabled;
	MultiMapMode multiMapMode = MultiMapMode::Map;
	bool ignoreStoredAttribute = false;

	mutable ConverterStore<TypeConverter> typeConverters;
	mutable ThreadSafeStore<TypeConverter> serCache;
	mutable ThreadSafeStore<TypeConverter> deserCache;

	template <typename TConverter>
	void insertSorted(const QSharedPointer<TConverter> &converter, QList<QSharedPointer<TConverter>> &list) const;

	QSharedPointer<TypeConverter> findSerConverter(int propertyType) const;
	QSharedPointer<TypeConverter> findDeserConverter(int &propertyType, QCborTag tag, QCborValue::Type type) const;
	void updateConverterStore() const;

	int getEnumId(QMetaEnum metaEnum, bool ser) const;
	virtual QCborValue serializeValue(int propertyType, const QVariant &value) const;
	virtual QVariant deserializeCborValue(int propertyType, const QCborValue &value) const;
	virtual QVariant deserializeJsonValue(int propertyType, const QCborValue &value) const;
};

Q_DECLARE_LOGGING_CATEGORY(logSerializer)
Q_DECLARE_LOGGING_CATEGORY(logSerializerExtractor)

template<typename TConverter>
SerializerBasePrivate::ThreadSafeStore<TConverter>::ThreadSafeStore(std::initializer_list<std::pair<int, QSharedPointer<TConverter>>> initData)
	: _store{std::move(initData)}
{}

template<typename TConverter>
QSharedPointer<TConverter> SerializerBasePrivate::ThreadSafeStore<TConverter>::get(int metaTypeId) const
{
	QReadLocker _{&_lock};
	return _store.value(metaTypeId, nullptr);
}

template<typename TConverter>
void SerializerBasePrivate::ThreadSafeStore<TConverter>::add(int metaTypeId, const QSharedPointer<TConverter> &converter)
{
	QWriteLocker _{&_lock};
	_store.insert(metaTypeId, converter);
}

template<typename TConverter>
void SerializerBasePrivate::ThreadSafeStore<TConverter>::clear()
{
	QWriteLocker _{&_lock};
	_store.clear();
}

template<typename TConverter>
SerializerBasePrivate::ConverterStore<TConverter>::ConverterStore(std::initializer_list<QSharedPointer<TConverter>> initData)
	: store{std::move(initData)}
{
#ifndef QT_NO_DEBUG
	for (auto i = 1; i < store.size(); ++i)
		Q_ASSERT(store[i]->priority() <= store[i - 1]->priority());
#endif
}

template<typename TConverter>
void SerializerBasePrivate::ConverterStore<TConverter>::insertSorted(const QSharedPointer<TConverter> &converter)
{
	QWriteLocker _{&lock};
	insertSorted(converter, _);
}

template<typename TConverter>
void SerializerBasePrivate::ConverterStore<TConverter>::insertSorted(const QSharedPointer<TConverter> &converter, QWriteLocker &locker)
{
	Q_UNUSED(locker)
	for (auto it = store.begin(); it != store.end(); ++it) {
		if ((*it)->priority() < converter->priority()) {
			store.insert(it, converter);
			return;
		}
	}
	// not inserted -> add to end
	store.append(converter);
}

}

#endif // QTJSONSERIALIZER_SERIALIZERBASE_P_H
