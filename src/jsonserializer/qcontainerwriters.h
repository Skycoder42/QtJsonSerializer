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

namespace QtJsonSerializer::MetaWriters {

class QSequentialWriterFactory;
class Q_JSONSERIALIZER_EXPORT QSequentialWriter
{
	Q_DISABLE_COPY(QSequentialWriter)

public:
	struct SequenceInfo {
		int type = QMetaType::UnknownType;
		bool isSet = false;
	};

	template <template<typename> class TContainer, typename TClass>
	static void registerWriter();
	static void registerWriter(int metaTypeId, QSequentialWriterFactory *factory);
	static bool canWrite(int metaTypeId);
	static QSharedPointer<QSequentialWriter> getWriter(QVariant &data);
	static SequenceInfo getInfo(int metaTypeId);

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



class QAssociativeWriterFactory;
class Q_JSONSERIALIZER_EXPORT QAssociativeWriter
{
	Q_DISABLE_COPY(QAssociativeWriter)

public:
	struct AssociationInfo {
		int keyType = QMetaType::UnknownType;
		int valueType = QMetaType::UnknownType;
	};

	template <template<typename, typename> class TContainer, typename TKey, typename TValue>
	static void registerWriter();
	static void registerWriter(int metaTypeId, QAssociativeWriterFactory *factory);
	static bool canWrite(int metaTypeId);
	static QSharedPointer<QAssociativeWriter> getWriter(QVariant &data);
	static AssociationInfo getInfo(int metaTypeId);

	virtual ~QAssociativeWriter();
	virtual AssociationInfo info() const = 0;
	virtual void add(const QVariant &key, const QVariant &value) = 0;

protected:
	QAssociativeWriter();
};

class Q_JSONSERIALIZER_EXPORT QAssociativeWriterFactory
{
	Q_DISABLE_COPY(QAssociativeWriterFactory)

public:
	QAssociativeWriterFactory();
	virtual ~QAssociativeWriterFactory();
	virtual QSharedPointer<QAssociativeWriter> create(void *data) const = 0;
};

// ------------- Generic Implementation classes -------------

template <template<typename> class TContainer, typename TClass>
class QSequentialWriterImpl final : public QSequentialWriter
{
public:
	QSequentialWriterImpl(TContainer<TClass> *data)
		: _data{data}
	{}

	SequenceInfo info() const final {
		return {qMetaTypeId<TClass>(), false};
	}

	void reserve(int size) final {
		_data->reserve(size);
	}

	void add(const QVariant &value) final {
		_data->append(value.template value<TClass>());
	}

private:
	TContainer<TClass> *_data;
};

template <template<typename> class TContainer, typename TClass>
class QSequentialWriterFactoryImpl final : public QSequentialWriterFactory
{
public:
	QSharedPointer<QSequentialWriter> create(void *data) const final {
		return QSharedPointer<QSequentialWriterImpl<TContainer, TClass>>::create(reinterpret_cast<TContainer<TClass>*>(data));
	}
};



template <template<typename, typename> class TContainer, typename TKey, typename TValue>
class QAssociativeWriterImpl final : public QAssociativeWriter
{
public:
	QAssociativeWriterImpl(TContainer<TKey, TValue> *data)
		: _data{data}
	{}

	AssociationInfo info() const final {
		return {qMetaTypeId<TKey>(), qMetaTypeId<TValue>()};
	}

	void add(const QVariant &key, const QVariant &value) final {
		_data->insert(key.template value<TKey>(),
					  value.template value<TValue>());
	}

private:
	TContainer<TKey, TValue> *_data;
};

template <template<typename, typename> class TContainer, typename TKey, typename TValue>
class QAssociativeWriterFactoryImpl final : public QAssociativeWriterFactory
{
public:
	QSharedPointer<QAssociativeWriter> create(void *data) const final {
		return QSharedPointer<QAssociativeWriterImpl<TContainer, TKey, TValue>>::create(reinterpret_cast<TContainer<TKey, TValue>*>(data));
	}
};

// ------------- Specializations and base generic implementations -------------

template <typename TClass>
class QSequentialWriterImpl<QSet, TClass> final : public QSequentialWriter
{
public:
	QSequentialWriterImpl(QSet<TClass> *data)
		: _data{data}
	{}

	SequenceInfo info() const final {
		return {qMetaTypeId<TClass>(), true};
	}

	void reserve(int size) final {
		_data->reserve(size);
	}

	void add(const QVariant &value) final {
		_data->insert(value.template value<TClass>());
	}

private:
	QSet<TClass> *_data;
};

template <typename TClass>
class QSequentialWriterImpl<QLinkedList, TClass> final : public QSequentialWriter
{
public:
	QSequentialWriterImpl(QLinkedList<TClass> *data)
		: _data{data}
	{}

	SequenceInfo info() const final {
		return {qMetaTypeId<TClass>(), false};
	}

	void reserve(int) final {}

	void add(const QVariant &value) final {
		_data->append(value.template value<TClass>());
	}

private:
	QLinkedList<TClass> *_data;
};

template <>
class QSequentialWriterImpl<QList, QVariant> final : public QSequentialWriter
{
public:
	QSequentialWriterImpl(QVariantList *data);

	SequenceInfo info() const final;
	void reserve(int size) final;
	void add(const QVariant &value) final;

private:
	QVariantList *_data;
};



template <>
class QAssociativeWriterImpl<QMap, QString, QVariant> final : public QAssociativeWriter
{
public:
	QAssociativeWriterImpl(QVariantMap *data);

	AssociationInfo info() const final;
	void add(const QVariant &key, const QVariant &value) final;

private:
	QVariantMap *_data;
};

template <>
class QAssociativeWriterImpl<QHash, QString, QVariant> final : public QAssociativeWriter
{
public:
	QAssociativeWriterImpl(QVariantHash *data);

	AssociationInfo info() const final;
	void add(const QVariant &key, const QVariant &value) final;

private:
	QVariantHash *_data;
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
	registerWriter(qMetaTypeId<TContainer<TKey, TValue>>(),
				   new QAssociativeWriterFactoryImpl<TContainer, TKey, TValue>{});
}

}

#endif // QCONTAINERWRITERS_H
