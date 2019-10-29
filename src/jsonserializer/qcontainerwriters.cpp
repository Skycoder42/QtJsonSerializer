#include "qcontainerwriters.h"
#include "qcontainerwriters_p.h"
#include <QDebug>

namespace {

class QSequentialWriterFactoryQStringList : public QSequentialWriterFactory
{
public:
	QSharedPointer<QSequentialWriter> create(void *data) const final {
		return QSharedPointer<QSequentialWriterImpl<QList, QString>>::create(reinterpret_cast<QStringList*>(data));
	}
};

class QSequentialWriterFactoryQByteArrayList : public QSequentialWriterFactory
{
public:
	QSharedPointer<QSequentialWriter> create(void *data) const final {
		return QSharedPointer<QSequentialWriterImpl<QList, QByteArray>>::create(reinterpret_cast<QByteArrayList*>(data));
	}
};

class QSequentialWriterFactoryQVariantList : public QSequentialWriterFactory
{
public:
	QSharedPointer<QSequentialWriter> create(void *data) const final {
		return QSharedPointer<QSequentialWriterImpl<QList, QVariant>>::create(reinterpret_cast<QVariantList*>(data));
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

}



void QSequentialWriter::registerWriter(int metaTypeId, QSequentialWriterFactory *factory)
{
	Q_ASSERT(factory);
	QWriteLocker _{&QContainerWritersPrivate::sequenceLock};
	QContainerWritersPrivate::sequenceFactories.insert(metaTypeId, factory);
}

bool QSequentialWriter::canWrite(int metaTypeId)
{
	QReadLocker _{&QContainerWritersPrivate::sequenceLock};
	return QContainerWritersPrivate::sequenceFactories.contains(metaTypeId);
}

QSharedPointer<QSequentialWriter> QSequentialWriter::getWriter(QVariant &data)
{
	QReadLocker _{&QContainerWritersPrivate::sequenceLock};
	const auto factory = QContainerWritersPrivate::sequenceFactories.value(data.userType());
	if (factory)
		return factory->create(data.data());
	else
		return {};
}

QSequentialWriter::SequenceInfo QSequentialWriter::getSequenceInfo(int metaTypeId)
{
	QReadLocker _{&QContainerWritersPrivate::sequenceLock};
	const auto factory = QContainerWritersPrivate::sequenceFactories.value(metaTypeId);
	if (factory)
		return factory->create(nullptr)->info();
	else
		return {};
}

QSequentialWriter::~QSequentialWriter() = default;

QSequentialWriter::QSequentialWriter() = default;



QSequentialWriterFactory::QSequentialWriterFactory() = default;

QSequentialWriterFactory::~QSequentialWriterFactory() = default;



QReadWriteLock QContainerWritersPrivate::sequenceLock;
QHash<int, QSequentialWriterFactory*> QContainerWritersPrivate::sequenceFactories {
	{QMetaType::QStringList, new QSequentialWriterFactoryQStringList{}},
	{QMetaType::QByteArrayList, new QSequentialWriterFactoryQByteArrayList{}},
	{QMetaType::QVariantList, new QSequentialWriterFactoryQVariantList{}}
};




QReadWriteLock QAssociativeWriterPrivateFactory::lock;
QHash<int, QAssociativeWriterPrivateFactory*> QAssociativeWriterPrivateFactory::factories {
	{QMetaType::QVariantMap, new QAssociativeWriterPrivateFactoryQVariantMap{}},
	{QMetaType::QVariantHash, new QAssociativeWriterPrivateFactoryQVariantHash{}}
};

bool QAssociativeWriter::canWrite(int metaTypeId)
{
	QReadLocker _{&QAssociativeWriterPrivateFactory::lock};
	return QAssociativeWriterPrivateFactory::factories.contains(metaTypeId);
}

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

std::pair<int, int> QAssociativeWriter::mapTypes() const
{
	return d->mapTypesImpl();
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
