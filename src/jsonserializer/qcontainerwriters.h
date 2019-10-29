#ifndef QCONTAINERWRITERS_H
#define QCONTAINERWRITERS_H

#include "QtJsonSerializer/qtjsonserializer_global.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qvariant.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qhash.h>
#include <QtCore/qreadwritelock.h>

#include <QtCore/qset.h>
#include <QtCore/qlinkedlist.h>

class QSequentialWriterFactory;
class Q_JSONSERIALIZER_EXPORT QSequentialWriter
{
	Q_DISABLE_COPY(QSequentialWriter)

public:
	struct SequenceInfo {
		int type;
		bool isSet;
	};

	template <template<typename> class TContainer, typename TClass>
	static void registerWriter();
	static void registerWriter(int metaTypeId, QSequentialWriterFactory *factory);
	static bool canWrite(int metaTypeId);
	static QSharedPointer<QSequentialWriter> getWriter(QVariant &data);
	static SequenceInfo getSequenceInfo(int metaTypeId);

	virtual ~QSequentialWriter();
	virtual SequenceInfo info() const = 0;
	virtual void reserve(int size) = 0;
	virtual void add(const QVariant &value) = 0;

protected:
	QSequentialWriter();
};

class Q_JSONSERIALIZER_EXPORT QSequentialWriterFactory
{
	Q_DISABLE_COPY(QSequentialWriterFactory)

public:
	QSequentialWriterFactory();
	virtual ~QSequentialWriterFactory();
	virtual QSharedPointer<QSequentialWriter> create(void *data) const = 0;
};

class QAssociativeWriterPrivate;
class Q_JSONSERIALIZER_EXPORT QAssociativeWriter
{
public:
	template <template<typename, typename> class TContainer, typename TKey, typename TValue>
	static void registerWriter();
	static bool canWrite(int metaTypeId);
	static QAssociativeWriter getWriter(QVariant &data);

	bool isValid() const;
	std::pair<int, int> mapTypes() const;
	void add(const QVariant &key, const QVariant &value);

private:
	QSharedPointer<QAssociativeWriterPrivate> d;

	QAssociativeWriter(QSharedPointer<QAssociativeWriterPrivate> &&dd = {});
};

// Generic Implementations

template <template<typename> class TContainer, typename TClass>
class QSequentialWriterImpl : public QSequentialWriter
{
public:
	QSequentialWriterImpl(TContainer<TClass> *data)
		: _data{data}
	{}

	SequenceInfo info() const override {
		return {qMetaTypeId<TClass>(), false};
	}

	void reserve(int size) override {
		_data->reserve(size);
	}

	void add(const QVariant &value) override {
		_data->append(value.template value<TClass>());
	}

private:
	TContainer<TClass> *_data;
};

template <template<typename> class TContainer, typename TClass>
class QSequentialWriterFactoryImpl : public QSequentialWriterFactory
{
public:
	QSharedPointer<QSequentialWriter> create(void *data) const final {
		return QSharedPointer<QSequentialWriterImpl<TContainer, TClass>>::create(reinterpret_cast<TContainer<TClass>*>(data));
	}
};

class Q_JSONSERIALIZER_EXPORT QAssociativeWriterPrivate
{
	Q_DISABLE_COPY(QAssociativeWriterPrivate)

public:
	QAssociativeWriterPrivate();
	virtual ~QAssociativeWriterPrivate();
	virtual std::pair<int, int> mapTypesImpl() const = 0;
	virtual void addImpl(const QVariant &key, const QVariant &value) = 0;
};

template <template<typename, typename> class TContainer, typename TKey, typename TValue>
class QAssociativeWriterPrivateImpl : public QAssociativeWriterPrivate
{
public:
	QAssociativeWriterPrivateImpl(TContainer<TKey, TValue> *data)
		: _data{data}
	{}

	std::pair<int, int> mapTypesImpl() const override {
		return std::make_pair(qMetaTypeId<TKey>(), qMetaTypeId<TValue>());
	}

	void addImpl(const QVariant &key, const QVariant &value) override {
		_data->insert(key.template value<TKey>(),
					  value.template value<TValue>());
	}

private:
	TContainer<TKey, TValue> *_data;
};

class Q_JSONSERIALIZER_EXPORT QAssociativeWriterPrivateFactory
{
	Q_DISABLE_COPY(QAssociativeWriterPrivateFactory)

public:
	static QReadWriteLock lock;
	static QHash<int, QAssociativeWriterPrivateFactory*> factories;

	QAssociativeWriterPrivateFactory();
	virtual ~QAssociativeWriterPrivateFactory();
	virtual QSharedPointer<QAssociativeWriterPrivate> create(void *data) const = 0;
};

template <template<typename, typename> class TContainer, typename TKey, typename TValue>
class QAssociativeWriterPrivateFactoryImpl : public QAssociativeWriterPrivateFactory
{
public:
	QSharedPointer<QAssociativeWriterPrivate> create(void *data) const final {
		return QSharedPointer<QAssociativeWriterPrivateImpl<TContainer, TKey, TValue>>::create(reinterpret_cast<TContainer<TKey, TValue>*>(data));
	}
};

// specializations and implementations

template <typename TClass>
class QSequentialWriterImpl<QSet, TClass> : public QSequentialWriter
{
public:
	QSequentialWriterImpl(QSet<TClass> *data)
		: _data{data}
	{}

	SequenceInfo info() const override {
		return {qMetaTypeId<TClass>(), true};
	}

	void reserve(int size) override {
		_data->reserve(size);
	}

	void add(const QVariant &value) override {
		_data->insert(value.template value<TClass>());
	}

private:
	QSet<TClass> *_data;
};

template <typename TClass>
class QSequentialWriterImpl<QLinkedList, TClass> : public QSequentialWriter
{
public:
	QSequentialWriterImpl(QLinkedList<TClass> *data)
		: _data{data}
	{}

	SequenceInfo info() const override {
		return {qMetaTypeId<TClass>(), false};
	}

	void reserve(int) override {}

	void add(const QVariant &value) override {
		_data->append(value.template value<TClass>());
	}

private:
	QLinkedList<TClass> *_data;
};

template<template<typename> class TContainer, typename TClass>
void QSequentialWriter::registerWriter()
{
	registerWriter(qMetaTypeId<TContainer<TClass>>(),
				   new QSequentialWriterFactoryImpl<TContainer, TClass>{});
}

template<template<typename, typename> class TContainer, typename TKey, typename TValue>
void QAssociativeWriter::registerWriter()
{
	QWriteLocker _{&QAssociativeWriterPrivateFactory::lock};
	QAssociativeWriterPrivateFactory::factories.insert(qMetaTypeId<TContainer<TKey, TValue>>(),
													   new QAssociativeWriterPrivateFactoryImpl<TContainer, TKey, TValue>{});
}

#endif // QCONTAINERWRITERS_H
