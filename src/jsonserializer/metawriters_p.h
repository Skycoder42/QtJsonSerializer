#ifndef QTJSONSERIALIZER_METAWRITERS_P_H
#define QTJSONSERIALIZER_METAWRITERS_P_H

#include "metawriters.h"

#include <QtCore/QLoggingCategory>

namespace QtJsonSerializer::MetaWriters {

class MetaWritersPrivate
{
public:
	static QReadWriteLock sequenceLock;
	static QHash<int, SequentialWriterFactory*> sequenceFactories;
	static QHash<int, SequentialWriter::SequenceInfo> sequenceInfoCache;

	static QReadWriteLock associationLock;
	static QHash<int, AssociativeWriterFactory*> associationFactories;
	static QHash<int, AssociativeWriter::AssociationInfo> associationInfoCache;

	static SequentialWriter::SequenceInfo tryParseSequenceInfo(int metaTypeId);
	static AssociativeWriter::AssociationInfo tryParseAssociationInfo(int metaTypeId);
};

Q_DECLARE_LOGGING_CATEGORY(logSeqWriter)
Q_DECLARE_LOGGING_CATEGORY(logAsocWriter)



class SequentialWriterFactoryQStringList : public SequentialWriterFactory
{
public:
	QSharedPointer<SequentialWriter> create(void *data) const final {
		return QSharedPointer<SequentialWriterImpl<QList, QString>>::create(reinterpret_cast<QStringList*>(data));
	}
};

class SequentialWriterFactoryQByteArrayList : public SequentialWriterFactory
{
public:
	QSharedPointer<SequentialWriter> create(void *data) const final {
		return QSharedPointer<SequentialWriterImpl<QList, QByteArray>>::create(reinterpret_cast<QByteArrayList*>(data));
	}
};

class SequentialWriterFactoryQVariantList : public SequentialWriterFactory
{
public:
	QSharedPointer<SequentialWriter> create(void *data) const final {
		return QSharedPointer<SequentialWriterImpl<QList, QVariant>>::create(reinterpret_cast<QVariantList*>(data));
	}
};



class AssociativeWriterFactoryQVariantMap : public AssociativeWriterFactory
{
public:
	QSharedPointer<AssociativeWriter> create(void *data) const final {
		return QSharedPointer<AssociativeWriterImpl<QMap, QString, QVariant>>::create(reinterpret_cast<QVariantMap*>(data));
	}
};

class AssociativeWriterFactoryQVariantHash : public AssociativeWriterFactory
{
public:
	QSharedPointer<AssociativeWriter> create(void *data) const final {
		return QSharedPointer<AssociativeWriterImpl<QHash, QString, QVariant>>::create(reinterpret_cast<QVariantHash*>(data));
	}
};

}

#endif // QTJSONSERIALIZER_METAWRITERS_P_H
