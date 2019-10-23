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

class QAssociativeWriterPrivateFactoryQVariantMap : public QAssociativeWriterPrivateFactory
{
public:
	QSharedPointer<QAssociativeWriterPrivate> create(void *data) const final {
		return QSharedPointer<QAssociativeWriterPrivateImpl<QMap, QString, QVariant>>::create(reinterpret_cast<QVariantMap*>(data));
	}
};

class QAssociativeWriterPrivateFactoryQVariantHash : public QAssociativeWriterPrivateFactory
{
public:
	QSharedPointer<QAssociativeWriterPrivate> create(void *data) const final {
		return QSharedPointer<QAssociativeWriterPrivateImpl<QHash, QString, QVariant>>::create(reinterpret_cast<QVariantHash*>(data));
	}
};

QReadWriteLock QSequentialWriterPrivateFactory::lock;
QHash<int, QSequentialWriterPrivateFactory*> QSequentialWriterPrivateFactory::factories {
	{QMetaType::QStringList, new QSequentialWriterPrivateFactoryQStringList{}},
	{QMetaType::QByteArrayList, new QSequentialWriterPrivateFactoryQByteArrayList{}},
	{QMetaType::QVariantList, new QSequentialWriterPrivateFactoryQVariantList{}}
};

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

QSequentialWriterPrivate::QSequentialWriterPrivate() = default;

QSequentialWriterPrivate::~QSequentialWriterPrivate() = default;

QSequentialWriterPrivateFactory::QSequentialWriterPrivateFactory() = default;

QSequentialWriterPrivateFactory::~QSequentialWriterPrivateFactory() = default;



QReadWriteLock QAssociativeWriterPrivateFactory::lock;
QHash<int, QAssociativeWriterPrivateFactory*> QAssociativeWriterPrivateFactory::factories {
	{QMetaType::QVariantMap, new QAssociativeWriterPrivateFactoryQVariantMap{}},
	{QMetaType::QVariantHash, new QAssociativeWriterPrivateFactoryQVariantHash{}}
};

QAssociativeWriter QAssociativeWriter::getWriter(QVariant &data)
{
	QReadLocker _{&QAssociativeWriterPrivateFactory::lock};
	const auto factory = QAssociativeWriterPrivateFactory::factories.value(data.userType());
	if (factory)
		return factory->create(data.data());
	else
		return {};
}

bool QAssociativeWriter::isValid() const
{
	return d;
}

void QAssociativeWriter::add(const QVariant &key, const QVariant &value)
{
	d->addImpl(key, value);
}

QAssociativeWriter::QAssociativeWriter(QSharedPointer<QAssociativeWriterPrivate> &&dd)
	: d{std::move(dd)}
{}

QAssociativeWriterPrivate::QAssociativeWriterPrivate() = default;

QAssociativeWriterPrivate::~QAssociativeWriterPrivate() = default;

QAssociativeWriterPrivateFactory::QAssociativeWriterPrivateFactory() = default;

QAssociativeWriterPrivateFactory::~QAssociativeWriterPrivateFactory() = default;
