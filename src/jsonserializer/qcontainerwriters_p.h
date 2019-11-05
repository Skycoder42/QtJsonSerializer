#ifndef QCONTAINERWRITERS_P_H
#define QCONTAINERWRITERS_P_H

#include "qcontainerwriters.h"

namespace QtJsonSerializer::MetaWriters {

class QContainerWritersPrivate
{
public:
	static QReadWriteLock sequenceLock;
	static QHash<int, QSequentialWriterFactory*> sequenceFactories;
	static QHash<int, QSequentialWriter::SequenceInfo> sequenceInfoCache;

	static QReadWriteLock associationLock;
	static QHash<int, QAssociativeWriterFactory*> associationFactories;
	static QHash<int, QAssociativeWriter::AssociationInfo> associationInfoCache;

	static QSequentialWriter::SequenceInfo tryParseSequenceInfo(int metaTypeId);
	static QAssociativeWriter::AssociationInfo tryParseAssociationInfo(int metaTypeId);
};



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



class QAssociativeWriterFactoryQVariantMap : public QAssociativeWriterFactory
{
public:
	QSharedPointer<QAssociativeWriter> create(void *data) const final {
		return QSharedPointer<QAssociativeWriterImpl<QMap, QString, QVariant>>::create(reinterpret_cast<QVariantMap*>(data));
	}
};

class QAssociativeWriterFactoryQVariantHash : public QAssociativeWriterFactory
{
public:
	QSharedPointer<QAssociativeWriter> create(void *data) const final {
		return QSharedPointer<QAssociativeWriterImpl<QHash, QString, QVariant>>::create(reinterpret_cast<QVariantHash*>(data));
	}
};

}

#endif // QCONTAINERWRITERS_P_H
