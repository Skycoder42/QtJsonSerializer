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

class QSequentialWriterPrivate;
class Q_JSONSERIALIZER_EXPORT QSequentialWriter
{
public:
	template <template<typename> class TContainer, typename TClass>
	static void registerWriter();
	static QSequentialWriter getWriter(QVariant &data);

	bool isValid() const;
	void reserve(int size);
	void add(const QVariant &value);

private:
	QSharedPointer<QSequentialWriterPrivate> d;

	QSequentialWriter(QSharedPointer<QSequentialWriterPrivate> &&dd = {});
};

// private stuff

class Q_JSONSERIALIZER_EXPORT QSequentialWriterPrivate
{
	Q_DISABLE_COPY(QSequentialWriterPrivate)

public:
	QSequentialWriterPrivate();
	virtual ~QSequentialWriterPrivate();
	virtual void reserveImpl(int size) = 0;
	virtual void addImpl(const QVariant &value) = 0;
};

template <template<typename> class TContainer, typename TClass>
class QSequentialWriterPrivateImpl : public QSequentialWriterPrivate
{
public:
	QSequentialWriterPrivateImpl(TContainer<TClass> *data)
		: _data{data}
	{}

	void reserveImpl(int size) override {
		_data->reserve(size);
	}
	void addImpl(const QVariant &value) override {
		_data->append(value.template value<TClass>());
	}

private:
	TContainer<TClass> *_data;
};

class Q_JSONSERIALIZER_EXPORT QSequentialWriterPrivateFactory
{
	Q_DISABLE_COPY(QSequentialWriterPrivateFactory)

public:
	static QReadWriteLock lock;
	static QHash<int, QSequentialWriterPrivateFactory*> factories;

	QSequentialWriterPrivateFactory() = default;
	virtual ~QSequentialWriterPrivateFactory() = default;
	virtual QSharedPointer<QSequentialWriterPrivate> create(void *data) const = 0;
};

template <template<typename> class TContainer, typename TClass>
class QSequentialWriterPrivateFactoryImpl : public QSequentialWriterPrivateFactory
{
public:
	QSharedPointer<QSequentialWriterPrivate> create(void *data) const final {
		return QSharedPointer<QSequentialWriterPrivateImpl<TContainer, TClass>>::create(reinterpret_cast<TContainer<TClass>*>(data));
	}
};

template<template<typename> class TContainer, typename TClass>
void QSequentialWriter::registerWriter()
{
	QWriteLocker _{&QSequentialWriterPrivateFactory::lock};
	QSequentialWriterPrivateFactory::factories.insert(qMetaTypeId<TContainer<TClass>>(),
													  new QSequentialWriterPrivateFactoryImpl<TContainer, TClass>{});
}

// specializations

template <typename TClass>
class QSequentialWriterPrivateImpl<QSet, TClass> : public QSequentialWriterPrivate
{
public:
	QSequentialWriterPrivateImpl(QSet<TClass> *data)
		: _data{data}
	{}

	void reserveImpl(int size) override {
		_data->reserve(size);
	}
	void addImpl(const QVariant &value) override {
		_data->insert(value.template value<TClass>());
	}

private:
	QSet<TClass> *_data;
};

template <typename TClass>
class QSequentialWriterPrivateImpl<QLinkedList, TClass> : public QSequentialWriterPrivate
{
public:
	QSequentialWriterPrivateImpl(QLinkedList<TClass> *data)
		: _data{data}
	{}

	void reserveImpl(int) override {}
	void addImpl(const QVariant &value) override {
		_data->append(value.template value<TClass>());
	}

private:
	QLinkedList<TClass> *_data;
};

template <>
class QSequentialWriterPrivateImpl<QList, QVariant> : public QSequentialWriterPrivate
{
public:
	QSequentialWriterPrivateImpl(QVariantList *data);

	void reserveImpl(int size) override;
	void addImpl(const QVariant &value) override;

private:
	QVariantList *_data;
};

#endif // QCONTAINERWRITERS_H
