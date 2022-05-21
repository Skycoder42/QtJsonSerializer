#ifndef QTJSONSERIALIZER_METAWRITERS_H
#define QTJSONSERIALIZER_METAWRITERS_H

#include "QtJsonSerializer/qtjsonserializer_global.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qvariant.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qhash.h>
#include <QtCore/qreadwritelock.h>

#include <QtCore/qset.h>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtCore/qlinkedlist.h>
#endif

namespace QtJsonSerializer::MetaWriters {

class SequentialWriterFactory;
//! The writer class for sequential containers
class Q_JSONSERIALIZER_EXPORT SequentialWriter
{
	Q_DISABLE_COPY(SequentialWriter)

public:
	//! Information about a sequential container
	struct SequenceInfo {
		//! The containers data type
		int type = QMetaType::UnknownType;
		//! Specifies if the container is a finite set
		bool isSet = false;
	};

	//! Registers a container factory for the given container and value classes
	template <template<typename> class TContainer, typename TClass>
	static void registerWriter();
	//! @copybrief SequentialWriter::registerWriter()
	static void registerWriter(int metaTypeId, SequentialWriterFactory *factory);
	//! Checks if a writer exists for the given type
	static bool canWrite(int metaTypeId);
	//! Returns a writer instance for the given data, or nullptr if none found
	static QSharedPointer<SequentialWriter> getWriter(QVariant &data);
	//! Returns the information details of the given type
	static SequenceInfo getInfo(int metaTypeId);

	virtual ~SequentialWriter();
	//! Return the information for the wrapped container
	virtual SequenceInfo info() const = 0;
	//! Reserves space for size elements in the container
	virtual void reserve(int size) = 0;
	//! Adds an element to the "end" of the container
	virtual void add(const QVariant &value) = 0;

protected:
	//! @private
	SequentialWriter();
};

//! A factory to create sequential writer instances from variant data
class Q_JSONSERIALIZER_EXPORT SequentialWriterFactory
{
	Q_DISABLE_COPY(SequentialWriterFactory)

public:
	SequentialWriterFactory();
	virtual ~SequentialWriterFactory();
	//! Factory method to create the instance. data can be null for read-only writers
	virtual QSharedPointer<SequentialWriter> create(void *data) const = 0;
};



class AssociativeWriterFactory;
//! The writer class for associative containers
class Q_JSONSERIALIZER_EXPORT AssociativeWriter
{
	Q_DISABLE_COPY(AssociativeWriter)

public:
	//! Information about a associative container
	struct AssociationInfo {
		//! The type of the associations keys
		int keyType = QMetaType::UnknownType;
		//! The type of the associations values
		int valueType = QMetaType::UnknownType;
	};

	//! Registers a container factory for the given container, key and value classes
	template <template<typename, typename> class TContainer, typename TKey, typename TValue>
	static void registerWriter();
	//! @copybrief AssociativeWriter::registerWriter()
	static void registerWriter(int metaTypeId, AssociativeWriterFactory *factory);
	//! Checks if a writer exists for the given type
	static bool canWrite(int metaTypeId);
	//! Returns a writer instance for the given data, or nullptr if none found
	static QSharedPointer<AssociativeWriter> getWriter(QVariant &data);
	//! Returns the information details of the given type
	static AssociationInfo getInfo(int metaTypeId);

	virtual ~AssociativeWriter();
	//! Return the information for the wrapped container
	virtual AssociationInfo info() const = 0;
	//! Inserts the given value for the given key into the container
	virtual void add(const QVariant &key, const QVariant &value) = 0;

protected:
	//! @private
	AssociativeWriter();
};

//! A factory to create associative writer instances from variant data
class Q_JSONSERIALIZER_EXPORT AssociativeWriterFactory
{
	Q_DISABLE_COPY(AssociativeWriterFactory)

public:
	AssociativeWriterFactory();
	virtual ~AssociativeWriterFactory();
	//! Factory method to create the instance. data can be null for read-only writers
	virtual QSharedPointer<AssociativeWriter> create(void *data) const = 0;
};

// ------------- Generic Implementation classes -------------

namespace Implementations {

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

#if !defined(QT_NO_LINKED_LIST) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
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
#endif

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

}

template<template<typename> class TContainer, typename TClass>
void SequentialWriter::registerWriter()
{
	registerWriter(qMetaTypeId<TContainer<TClass>>(),
				   new Implementations::SequentialWriterFactoryImpl<TContainer, TClass>{});
}

template<template<typename, typename> class TContainer, typename TKey, typename TValue>
void AssociativeWriter::registerWriter()
{
	registerWriter(qMetaTypeId<TContainer<TKey, TValue>>(),
				   new Implementations::AssociativeWriterFactoryImpl<TContainer, TKey, TValue>{});
}

}

#endif // QTJSONSERIALIZER_METAWRITERS_H
