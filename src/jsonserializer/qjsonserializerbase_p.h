#ifndef QJSONSERIALIZERBASE_P_H
#define QJSONSERIALIZERBASE_P_H

#include "qtjsonserializer_global.h"
#include "qjsonserializerbase.h"

#include <QtCore/QReadWriteLock>
#include <QtCore/QHash>

#include <QtCore/private/qobject_p.h>

class Q_JSONSERIALIZER_EXPORT QJsonSerializerBasePrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(QJsonSerializerBase)

public:
	using ValidationFlag = QJsonSerializerBase::ValidationFlag;
	using ValidationFlags = QJsonSerializerBase::ValidationFlags;
	using Polymorphing = QJsonSerializerBase::Polymorphing;
	using MultiMapMode = QJsonSerializerBase::MultiMapMode;

	static QByteArray getTypeName(int propertyType);

	static QReadWriteLock typedefLock;
	static QHash<int, QByteArray> typedefMapping;

	static QReadWriteLock factoryLock;
	static QList<QSharedPointer<QJsonTypeConverterFactory>> typeConverterFactories;

	bool allowNull = false;
	bool keepObjectName = false;
	bool enumAsString = false;
	bool useBcp47Locale = true;
	ValidationFlags validationFlags = ValidationFlag::StandardValidation;
	Polymorphing polymorphing = Polymorphing::Enabled;
	MultiMapMode multiMapMode = MultiMapMode::Map;
	bool ignoreStoredAttribute = false;

	mutable QReadWriteLock typeConverterLock {};
	mutable QList<QSharedPointer<QJsonTypeConverter>> typeConverters;
	mutable QHash<int, QSharedPointer<QJsonTypeConverter>> typeConverterSerCache;
	mutable QHash<int, QSharedPointer<QJsonTypeConverter>> typeConverterDeserCache;

	QSharedPointer<QJsonTypeConverter> findSerConverter(int propertyType) const;
	QSharedPointer<QJsonTypeConverter> findDeserConverter(int &propertyType, QCborTag tag, QCborValue::Type type) const;

	QCborValue serializeValue(int propertyType, const QVariant &value) const;
	QVariant deserializeCborValue(int propertyType, const QCborValue &value) const;
	QVariant deserializeJsonValue(int propertyType, const QCborValue &value) const;
};

#endif // QJSONSERIALIZERBASE_P_H
