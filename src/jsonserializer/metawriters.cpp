#include "metawriters.h"
#include "metawriters_p.h"

#include <QtCore/QRegularExpression>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::MetaWriters;
using namespace QtJsonSerializer::MetaWriters::Implementations;

Q_LOGGING_CATEGORY(QtJsonSerializer::MetaWriters::logSeqWriter, "qt.jsonserializer.metawriters.sequential")
Q_LOGGING_CATEGORY(QtJsonSerializer::MetaWriters::logAsocWriter, "qt.jsonserializer.metawriters.associative")

void SequentialWriter::registerWriter(int metaTypeId, SequentialWriterFactory *factory)
{
	Q_ASSERT_X(factory, Q_FUNC_INFO, "factory must not be null!");
	QWriteLocker _{&MetaWritersPrivate::sequenceLock};
	MetaWritersPrivate::sequenceFactories.insert(metaTypeId, factory);
	MetaWritersPrivate::sequenceInfoCache.remove(metaTypeId);
	qCDebug(logSeqWriter) << "Added factory for type:" << QMetaTypeName(metaTypeId);
}

bool SequentialWriter::canWrite(int metaTypeId)
{
	QReadLocker _{&MetaWritersPrivate::sequenceLock};
	return MetaWritersPrivate::sequenceFactories.contains(metaTypeId);
}

QSharedPointer<SequentialWriter> SequentialWriter::getWriter(QVariant &data)
{
	QReadLocker _{&MetaWritersPrivate::sequenceLock};
	const auto factory = MetaWritersPrivate::sequenceFactories.value(data.userType());
	if (factory) {
		qCDebug(logSeqWriter) << "Found factory for data of type:" << QMetaTypeName(data.userType());
		return factory->create(data.data());
	} else {
		qCWarning(logSeqWriter) << "Unable to find factory for data of type:" << QMetaTypeName(data.userType());
		return {};
	}
}

SequentialWriter::SequenceInfo SequentialWriter::getInfo(int metaTypeId)
{
	QReadLocker rLocker{&MetaWritersPrivate::sequenceLock};
	auto it = MetaWritersPrivate::sequenceInfoCache.find(metaTypeId);
	if (it != MetaWritersPrivate::sequenceInfoCache.end()) {
		qCDebug(logSeqWriter) << "Found SequenceInfo for type" << QMetaTypeName(metaTypeId)
							  << "in cache";
		return *it;
	} else {
		rLocker.unlock();
		QWriteLocker wLocker{&MetaWritersPrivate::sequenceLock};
		const auto factory = MetaWritersPrivate::sequenceFactories.value(metaTypeId);
		if (factory) {
			qCDebug(logSeqWriter) << "Found factory to generate SequenceInfo for type:" << QMetaTypeName(metaTypeId);
			it = MetaWritersPrivate::sequenceInfoCache.insert(metaTypeId, factory->create(nullptr)->info());
		} else {
			qCWarning(logSeqWriter) << "Unable to find SequenceInfo for type" << QMetaTypeName(metaTypeId)
									<< "- trying to guess by parsing the types name";
			it = MetaWritersPrivate::sequenceInfoCache.insert(metaTypeId, MetaWritersPrivate::tryParseSequenceInfo(metaTypeId));
		}
		return *it;
	}
}

SequentialWriter::~SequentialWriter() = default;

SequentialWriter::SequentialWriter() = default;



SequentialWriterFactory::SequentialWriterFactory() = default;

SequentialWriterFactory::~SequentialWriterFactory() = default;



void AssociativeWriter::registerWriter(int metaTypeId, AssociativeWriterFactory *factory)
{
	Q_ASSERT_X(factory, Q_FUNC_INFO, "factory must not be null!");
	QWriteLocker _{&MetaWritersPrivate::associationLock};
	MetaWritersPrivate::associationFactories.insert(metaTypeId, factory);
	MetaWritersPrivate::associationInfoCache.remove(metaTypeId);
	qCDebug(logAsocWriter) << "Added factory for type:" << QMetaTypeName(metaTypeId);
}

bool AssociativeWriter::canWrite(int metaTypeId)
{
	QReadLocker _{&MetaWritersPrivate::associationLock};
	return MetaWritersPrivate::associationFactories.contains(metaTypeId);
}

QSharedPointer<AssociativeWriter> AssociativeWriter::getWriter(QVariant &data)
{
	QReadLocker _{&MetaWritersPrivate::associationLock};
	const auto factory = MetaWritersPrivate::associationFactories.value(data.userType());
	if (factory) {
		qCDebug(logAsocWriter) << "Found factory for data of type:" << QMetaTypeName(data.userType());
		return factory->create(data.data());
	} else {
		qCWarning(logAsocWriter) << "Unable to find factory for data of type:" << QMetaTypeName(data.userType());
		return {};
	}
}

AssociativeWriter::AssociationInfo AssociativeWriter::getInfo(int metaTypeId)
{
	QReadLocker rLocker{&MetaWritersPrivate::associationLock};
	auto it = MetaWritersPrivate::associationInfoCache.find(metaTypeId);
	if (it != MetaWritersPrivate::associationInfoCache.end()) {
		qCDebug(logAsocWriter) << "Found SequenceInfo for type" << QMetaTypeName(metaTypeId)
							   << "in cache";
		return *it;
	} else {
		rLocker.unlock();
		QWriteLocker wLocker{&MetaWritersPrivate::associationLock};
		const auto factory = MetaWritersPrivate::associationFactories.value(metaTypeId);
		if (factory) {
			qCDebug(logAsocWriter) << "Found factory to generate SequenceInfo for type:" << QMetaTypeName(metaTypeId);
			it = MetaWritersPrivate::associationInfoCache.insert(metaTypeId, factory->create(nullptr)->info());
		} else {
			qCWarning(logAsocWriter) << "Unable to find SequenceInfo for type" << QMetaTypeName(metaTypeId)
									 << "- trying to guess by parsing the types name";
			it = MetaWritersPrivate::associationInfoCache.insert(metaTypeId, MetaWritersPrivate::tryParseAssociationInfo(metaTypeId));
		}
		return *it;
	}
}

AssociativeWriter::~AssociativeWriter() = default;

AssociativeWriter::AssociativeWriter() = default;



AssociativeWriterFactory::AssociativeWriterFactory() = default;

AssociativeWriterFactory::~AssociativeWriterFactory() = default;



SequentialWriterImpl<QList, QVariant>::SequentialWriterImpl(QVariantList *data)
	: _data{data}
{}

SequentialWriter::SequenceInfo SequentialWriterImpl<QList, QVariant>::info() const
{
	return {QMetaType::UnknownType, false};
}

void SequentialWriterImpl<QList, QVariant>::reserve(int size)
{
	_data->reserve(size);
}

void SequentialWriterImpl<QList, QVariant>::add(const QVariant &value)
{
	_data->append(value);
}



AssociativeWriterImpl<QMap, QString, QVariant>::AssociativeWriterImpl(QVariantMap *data)
	: _data{data}
{}

AssociativeWriter::AssociationInfo AssociativeWriterImpl<QMap, QString, QVariant>::info() const
{
	return {QMetaType::QString, QMetaType::UnknownType};
}

void AssociativeWriterImpl<QMap, QString, QVariant>::add(const QVariant &key, const QVariant &value)
{
	_data->insert(key.toString(), value);
}

AssociativeWriterImpl<QHash, QString, QVariant>::AssociativeWriterImpl(QVariantHash *data)
	: _data{data}
{}

AssociativeWriter::AssociationInfo AssociativeWriterImpl<QHash, QString, QVariant>::info() const
{
	return {QMetaType::QString, QMetaType::UnknownType};
}

void AssociativeWriterImpl<QHash, QString, QVariant>::add(const QVariant &key, const QVariant &value)
{
	_data->insert(key.toString(), value);
}

// ------------- private implementation -------------

QReadWriteLock MetaWritersPrivate::sequenceLock;
QHash<int, SequentialWriterFactory*> MetaWritersPrivate::sequenceFactories {
	{QMetaType::QStringList, new SequentialWriterFactoryQStringList{}},
	{QMetaType::QByteArrayList, new SequentialWriterFactoryQByteArrayList{}},
	{QMetaType::QVariantList, new SequentialWriterFactoryQVariantList{}}
};
QHash<int, SequentialWriter::SequenceInfo> MetaWritersPrivate::sequenceInfoCache;

QReadWriteLock MetaWritersPrivate::associationLock;
QHash<int, AssociativeWriterFactory*> MetaWritersPrivate::associationFactories {
	{QMetaType::QVariantMap, new AssociativeWriterFactoryQVariantMap{}},
	{QMetaType::QVariantHash, new AssociativeWriterFactoryQVariantHash{}}
};
QHash<int, AssociativeWriter::AssociationInfo> MetaWritersPrivate::associationInfoCache;

SequentialWriter::SequenceInfo MetaWritersPrivate::tryParseSequenceInfo(int metaTypeId)
{
	if (metaTypeId == QMetaType::QStringList)
		return {QMetaType::QString, false};
	else if (metaTypeId == QMetaType::QByteArrayList)
		return {QMetaType::QByteArray, false};
	else if (metaTypeId == QMetaType::QVariantList)
		return {QMetaType::UnknownType, false};
	else {
		static const QRegularExpression listTypeRegex{
			QStringLiteral(R"__(^((?![0-9_])(?:\w|::)*\w)\s*<\s*(.*?)\s*>$)__"),
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto match = listTypeRegex.match(QString::fromUtf8(QMetaTypeName(metaTypeId)));
		if (match.hasMatch()) {
			return {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
				QMetaType::type(match.captured(2).toUtf8().trimmed()),
#else
				QMetaType::fromName(match.captured(2).toUtf8().trimmed()).id(),
#endif
								match.captured(1) == QStringLiteral("QSet")
			};
		}
	}

	return {QMetaType::UnknownType, false};
}

AssociativeWriter::AssociationInfo MetaWritersPrivate::tryParseAssociationInfo(int metaTypeId)
{
	if (metaTypeId == QMetaType::QVariantMap ||
		metaTypeId == QMetaType::QVariantHash)
		return {QMetaType::QString, QMetaType::UnknownType};
	else {
		static const QRegularExpression mapTypeRegex{
			QStringLiteral(R"__(^(?![0-9_])(?:\w|::)*\w\s*<\s*(.*?)\s*>$)__"),
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto match = mapTypeRegex.match(QString::fromUtf8(QMetaTypeName(metaTypeId)));
		if (match.hasMatch()) {
			// parse match, using <> and , rules
			const auto matchStr = match.captured(1);
			auto bCount = 0;
			for (auto i = 0; i < matchStr.size(); ++i) {
				const auto &c = matchStr[i];
				if (c == QLatin1Char('<'))
					++bCount;
				else if (c == QLatin1Char('>'))
					--bCount;
				else if (bCount == 0 && c == QLatin1Char(',')) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
					return {
						QMetaType::type(matchStr.mid(0, i).trimmed().toUtf8()),
						QMetaType::type(matchStr.mid(i + 1).trimmed().toUtf8())
					};
#else
					return {
						QMetaType::fromName(matchStr.mid(0, i).trimmed().toUtf8()).id(),
						QMetaType::fromName(matchStr.mid(i + 1).trimmed().toUtf8()).id()
					};

#endif
				}
			}
		}
	}

	return {QMetaType::UnknownType, QMetaType::UnknownType};
}
