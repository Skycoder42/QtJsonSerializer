#include "qcontainerwriters.h"
#include <QDebug>

class QSequentialWriterPrivateFactoryQStringList : public QSequentialWriterPrivateFactory
{
public:
	QSharedPointer<QSequentialWriterPrivate> create(void *data) const final {
		return QSharedPointer<QSequentialWriterPrivateImpl<QList, QString>>::create(reinterpret_cast<QStringList*>(data));
	}
};

class QSequentialWriterPrivateFactoryQByteArrayList : public QSequentialWriterPrivateFactory
{
public:
	QSharedPointer<QSequentialWriterPrivate> create(void *data) const final {
		return QSharedPointer<QSequentialWriterPrivateImpl<QList, QByteArray>>::create(reinterpret_cast<QByteArrayList*>(data));
	}
};

class QSequentialWriterPrivateFactoryQVariantList : public QSequentialWriterPrivateFactory
{
public:
	QSharedPointer<QSequentialWriterPrivate> create(void *data) const final {
		return QSharedPointer<QSequentialWriterPrivateImpl<QList, QVariant>>::create(reinterpret_cast<QVariantList*>(data));
	}
};

QReadWriteLock QSequentialWriterPrivateFactory::lock;
QHash<int, QSequentialWriterPrivateFactory*> QSequentialWriterPrivateFactory::factories {
	{QMetaType::QStringList, new QSequentialWriterPrivateFactoryQStringList{}},
	{QMetaType::QByteArrayList, new QSequentialWriterPrivateFactoryQByteArrayList{}},
	{QMetaType::QVariantList, new QSequentialWriterPrivateFactoryQVariantList{}}
};

QSequentialWriterPrivate::QSequentialWriterPrivate() = default;

QSequentialWriterPrivate::~QSequentialWriterPrivate() = default;

QSequentialWriter QSequentialWriter::getWriter(QVariant &data)
{
	QReadLocker _{&QSequentialWriterPrivateFactory::lock};
	const auto factory = QSequentialWriterPrivateFactory::factories.value(data.userType());
	if (factory)
		return factory->create(data.data());
	else
		return {};
}

bool QSequentialWriter::isValid() const
{
	return d;
}

void QSequentialWriter::reserve(int size)
{
	d->reserveImpl(size);
}

void QSequentialWriter::add(const QVariant &value)
{
	d->addImpl(value);
}

QSequentialWriter::QSequentialWriter(QSharedPointer<QSequentialWriterPrivate> &&dd)
	: d{std::move(dd)}
{}



QSequentialWriterPrivateImpl<QList, QVariant>::QSequentialWriterPrivateImpl(QVariantList *data)
	: _data{data}
{}

void QSequentialWriterPrivateImpl<QList, QVariant>::reserveImpl(int size)
{
	_data->reserve(size);
}

void QSequentialWriterPrivateImpl<QList, QVariant>::addImpl(const QVariant &value)
{
	_data->append(value);
}
