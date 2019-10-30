#include "qcontainerwriters.h"
#include "qcontainerwriters_p.h"

#include <QtCore/QRegularExpression>

void QSequentialWriter::registerWriter(int metaTypeId, QSequentialWriterFactory *factory)
{
	Q_ASSERT(factory);
	QWriteLocker _{&QContainerWritersPrivate::sequenceLock};
	QContainerWritersPrivate::sequenceFactories.insert(metaTypeId, factory);
	QContainerWritersPrivate::sequenceInfoCache.remove(metaTypeId);
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

QSequentialWriter::SequenceInfo QSequentialWriter::getInfo(int metaTypeId)
{
	QReadLocker rLocker{&QContainerWritersPrivate::sequenceLock};
	auto it = QContainerWritersPrivate::sequenceInfoCache.find(metaTypeId);
	if (it != QContainerWritersPrivate::sequenceInfoCache.end())
		return *it;
	else {
		rLocker.unlock();
		QWriteLocker wLocker{&QContainerWritersPrivate::sequenceLock};
		const auto factory = QContainerWritersPrivate::sequenceFactories.value(metaTypeId);
		if (factory)
			it = QContainerWritersPrivate::sequenceInfoCache.insert(metaTypeId, factory->create(nullptr)->info());
		else
			it = QContainerWritersPrivate::sequenceInfoCache.insert(metaTypeId, QContainerWritersPrivate::tryParseSequenceInfo(metaTypeId));
		return *it;
	}
}

QSequentialWriter::~QSequentialWriter() = default;

QSequentialWriter::QSequentialWriter() = default;



QSequentialWriterFactory::QSequentialWriterFactory() = default;

QSequentialWriterFactory::~QSequentialWriterFactory() = default;



void QAssociativeWriter::registerWriter(int metaTypeId, QAssociativeWriterFactory *factory)
{
	Q_ASSERT(factory);
	QWriteLocker _{&QContainerWritersPrivate::associationLock};
	QContainerWritersPrivate::associationFactories.insert(metaTypeId, factory);
	QContainerWritersPrivate::associationInfoCache.remove(metaTypeId);
}

bool QAssociativeWriter::canWrite(int metaTypeId)
{
	QReadLocker _{&QContainerWritersPrivate::associationLock};
	return QContainerWritersPrivate::associationFactories.contains(metaTypeId);
}

QSharedPointer<QAssociativeWriter> QAssociativeWriter::getWriter(QVariant &data)
{
	QReadLocker _{&QContainerWritersPrivate::associationLock};
	const auto factory = QContainerWritersPrivate::associationFactories.value(data.userType());
	if (factory)
		return factory->create(data.data());
	else
		return {};
}

QAssociativeWriter::AssociationInfo QAssociativeWriter::getInfo(int metaTypeId)
{
	QReadLocker rLocker{&QContainerWritersPrivate::associationLock};
	auto it = QContainerWritersPrivate::associationInfoCache.find(metaTypeId);
	if (it != QContainerWritersPrivate::associationInfoCache.end())
		return *it;
	else {
		rLocker.unlock();
		QWriteLocker wLocker{&QContainerWritersPrivate::associationLock};
		const auto factory = QContainerWritersPrivate::associationFactories.value(metaTypeId);
		if (factory)
			it = QContainerWritersPrivate::associationInfoCache.insert(metaTypeId, factory->create(nullptr)->info());
		else
			it = QContainerWritersPrivate::associationInfoCache.insert(metaTypeId, QContainerWritersPrivate::tryParseAssociationInfo(metaTypeId));
		return *it;
	}
}

QAssociativeWriter::~QAssociativeWriter() = default;

QAssociativeWriter::QAssociativeWriter() = default;



QAssociativeWriterFactory::QAssociativeWriterFactory() = default;

QAssociativeWriterFactory::~QAssociativeWriterFactory() = default;



QSequentialWriterImpl<QList, QVariant>::QSequentialWriterImpl(QVariantList *data)
	: _data{data}
{}

QSequentialWriter::SequenceInfo QSequentialWriterImpl<QList, QVariant>::info() const
{
	return {QMetaType::UnknownType, false};
}

void QSequentialWriterImpl<QList, QVariant>::reserve(int size)
{
	_data->reserve(size);
}

void QSequentialWriterImpl<QList, QVariant>::add(const QVariant &value)
{
	_data->append(value);
}



QAssociativeWriterImpl<QMap, QString, QVariant>::QAssociativeWriterImpl(QVariantMap *data)
	: _data{data}
{}

QAssociativeWriter::AssociationInfo QAssociativeWriterImpl<QMap, QString, QVariant>::info() const
{
	return {QMetaType::QString, QMetaType::UnknownType};
}

void QAssociativeWriterImpl<QMap, QString, QVariant>::add(const QVariant &key, const QVariant &value)
{
	_data->insert(key.toString(), value);
}

QAssociativeWriterImpl<QHash, QString, QVariant>::QAssociativeWriterImpl(QVariantHash *data)
	: _data{data}
{}

QAssociativeWriter::AssociationInfo QAssociativeWriterImpl<QHash, QString, QVariant>::info() const
{
	return {QMetaType::QString, QMetaType::UnknownType};
}

void QAssociativeWriterImpl<QHash, QString, QVariant>::add(const QVariant &key, const QVariant &value)
{
	_data->insert(key.toString(), value);
}

// ------------- private implementation -------------

QReadWriteLock QContainerWritersPrivate::sequenceLock;
QHash<int, QSequentialWriterFactory*> QContainerWritersPrivate::sequenceFactories {
	{QMetaType::QStringList, new QSequentialWriterFactoryQStringList{}},
	{QMetaType::QByteArrayList, new QSequentialWriterFactoryQByteArrayList{}},
	{QMetaType::QVariantList, new QSequentialWriterFactoryQVariantList{}}
};
QHash<int, QSequentialWriter::SequenceInfo> QContainerWritersPrivate::sequenceInfoCache;

QReadWriteLock QContainerWritersPrivate::associationLock;
QHash<int, QAssociativeWriterFactory*> QContainerWritersPrivate::associationFactories {
	{QMetaType::QVariantMap, new QAssociativeWriterFactoryQVariantMap{}},
	{QMetaType::QVariantHash, new QAssociativeWriterFactoryQVariantHash{}}
};
QHash<int, QAssociativeWriter::AssociationInfo> QContainerWritersPrivate::associationInfoCache;

QSequentialWriter::SequenceInfo QContainerWritersPrivate::tryParseSequenceInfo(int metaTypeId)
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
		auto match = listTypeRegex.match(QString::fromUtf8(QMetaType::typeName(metaTypeId)));
		if (match.hasMatch()) {
			return {
				QMetaType::type(match.captured(2).toUtf8().trimmed()),
				match.captured(1) == QStringLiteral("QSet")
			};
		}
	}

	return {QMetaType::UnknownType, false};
}

QAssociativeWriter::AssociationInfo QContainerWritersPrivate::tryParseAssociationInfo(int metaTypeId)
{
	if (metaTypeId == QMetaType::QVariantMap ||
		metaTypeId == QMetaType::QVariantHash)
		return {QMetaType::QString, QMetaType::UnknownType};
	else {
		static const QRegularExpression mapTypeRegex{
			QStringLiteral(R"__(^(?![0-9_])(?:\w|::)*\w\s*<\s*(.*?)\s*>$)__"),
			QRegularExpression::UseUnicodePropertiesOption
		};
		auto match = mapTypeRegex.match(QString::fromUtf8(QMetaType::typeName(metaTypeId)));
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
					return {
						QMetaType::type(matchStr.mid(0, i).trimmed().toUtf8()),
						QMetaType::type(matchStr.mid(i + 1).trimmed().toUtf8())
					};
				}
			}
		}
	}

	return {QMetaType::UnknownType, QMetaType::UnknownType};
}
