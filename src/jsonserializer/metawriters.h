#ifndef QTJSONSERIALIZER_METAWRITERS_H
#define QTJSONSERIALIZER_METAWRITERS_H

#include "QtJsonSerializer/qtjsonserializer_global.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qvariant.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qhash.h>
#include <QtCore/qreadwritelock.h>

#include <QtCore/qset.h>
#include <QtCore/qlinkedlist.h>

namespace QtJsonSerializer::MetaWriters {

class SequentialWriterFactory;
class Q_JSONSERIALIZER_EXPORT SequentialWriter
{
	Q_DISABLE_COPY(SequentialWriter)

public:
	struct SequenceInfo {
		int type = QMetaType::UnknownType;
		bool isSet = false;
	};

	template <template<typename> class TContainer, typename TClass>
	static void registerWriter();
	static void registerWriter(int metaTypeId, SequentialWriterFactory *factory);
	static bool canWrite(int metaTypeId);
	static QSharedPointer<SequentialWriter> getWriter(QVariant &data);
	static SequenceInfo getInfo(int metaTypeId);

	virtual ~SequentialWriter();
	virtual SequenceInfo info() const = 0;
	virtual void reserve(int size) = 0;
	virtual void add(const QVariant &value) = 0;

protected:
	SequentialWriter();
};

class Q_JSONSERIALIZER_EXPORT SequentialWriterFactory
{
	Q_DISABLE_COPY(SequentialWriterFactory)

public:
	SequentialWriterFactory();
	virtual ~SequentialWriterFactory();
	virtual QSharedPointer<SequentialWriter> create(void *data) const = 0;
};



class AssociativeWriterFactory;
class Q_JSONSERIALIZER_EXPORT AssociativeWriter
{
	Q_DISABLE_COPY(AssociativeWriter)

public:
	struct AssociationInfo {
		int keyType = QMetaType::UnknownType;
		int valueType = QMetaType::UnknownType;
	};

	template <template<typename, typename> class TContainer, typename TKey, typename TValue>
	static void registerWriter();
	static void registerWriter(int metaTypeId, AssociativeWriterFactory *factory);
	static bool canWrite(int metaTypeId);
	static QSharedPointer<AssociativeWriter> getWriter(QVariant &data);
	static AssociationInfo getInfo(int metaTypeId);

	virtual ~AssociativeWriter();
	virtual AssociationInfo info() const = 0;
	virtual void add(const QVariant &key, const QVariant &value) = 0;

protected:
	AssociativeWriter();
};

class Q_JSONSERIALIZER_EXPORT AssociativeWriterFactory
{
	Q_DISABLE_COPY(AssociativeWriterFactory)

public:
	AssociativeWriterFactory();
	virtual ~AssociativeWriterFactory();
	virtual QSharedPointer<AssociativeWriter> create(void *data) const = 0;
};

// ------------- Generic Implementation classes -------------

template <template<typename> class TContainer, typename TClass>
class SequentialWriterImpl final : public SequentialWriter
{
public:
	SequentialWriterImpl(TContainer<TClass> *data)
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
class SequentialWriterFactoryImpl final : public SequentialWriterFactory
{
public:
	QSharedPointer<SequentialWriter> create(void *data) const final {
		return QSharedPointer<SequentialWriterImpl<TContainer, TClass>>::create(reinterpret_cast<TContainer<TClass>*>(data));
	}
};



template <template<typename, typename> class TContainer, typename TKey, typename TValue>
class AssociativeWriterImpl final : public AssociativeWriter
{
public:
	AssociativeWriterImpl(TContainer<TKey, TValue> *data)
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
class AssociativeWriterFactoryImpl final : public AssociativeWriterFactory
{
public:
	QSharedPointer<AssociativeWriter> create(void *data) const final {
		return QSharedPointer<AssociativeWriterImpl<TContainer, TKey, TValue>>::create(reinterpret_cast<TContainer<TKey, TValue>*>(data));
	}
};

// ------------- Specializations and base generic implementations -------------

template <typename TClass>
class SequentialWriterImpl<QSet, TClass> final : public SequentialWriter
{
public:
	SequentialWriterImpl(QSet<TClass> *data)
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
class SequentialWriterImpl<QLinkedList, TClass> final : public SequentialWriter
{
public:
	SequentialWriterImpl(QLinkedList<TClass> *data)
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
class SequentialWriterImpl<QList, QVariant> final : public SequentialWriter
{
public:
	SequentialWriterImpl(QVariantList *data);

	SequenceInfo info() const final;
	void reserve(int size) final;
	void add(const QVariant &value) final;

private:
	QVariantList *_data;
};



template <>
class AssociativeWriterImpl<QMap, QString, QVariant> final : public AssociativeWriter
{
public:
	AssociativeWriterImpl(QVariantMap *data);

	AssociationInfo info() const final;
	void add(const QVariant &key, const QVariant &value) final;

private:
	QVariantMap *_data;
};

template <>
class AssociativeWriterImpl<QHash, QString, QVariant> final : public AssociativeWriter
{
public:
	AssociativeWriterImpl(QVariantHash *data);

	AssociationInfo info() const final;
	void add(const QVariant &key, const QVariant &value) final;

private:
	QVariantHash *_data;
};



template<template<typename> class TContainer, typename TClass>
void SequentialWriter::registerWriter()
{
	registerWriter(qMetaTypeId<TContainer<TClass>>(),
				   new SequentialWriterFactoryImpl<TContainer, TClass>{});
}

template<template<typename, typename> class TContainer, typename TKey, typename TValue>
void AssociativeWriter::registerWriter()
{
	registerWriter(qMetaTypeId<TContainer<TKey, TValue>>(),
				   new AssociativeWriterFactoryImpl<TContainer, TKey, TValue>{});
}

}

#endif // QTJSONSERIALIZER_METAWRITERS_H
