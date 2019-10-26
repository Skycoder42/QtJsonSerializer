#ifndef DUMMYSERIALIZATIONHELPER_H
#define DUMMYSERIALIZATIONHELPER_H

#include <QtCore/QQueue>
#include <QtCore/QHash>
#include <QtJsonSerializer/QJsonTypeConverter>

class DummySerializationHelper : public QObject, public QJsonTypeConverter::SerializationHelper
{
	Q_OBJECT

public:
	DummySerializationHelper(QObject *parent = nullptr);

	bool jsonMode() const override;
	QVariant getProperty(const char *name) const override;
	QCborTag typeTag(int metaTypeId) const override;
	QByteArray getCanonicalTypeName(int propertyType) const override;
	QCborValue serializeSubtype(const QMetaProperty &property, const QVariant &value) const override;
	QCborValue serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const override;
	QVariant deserializeSubtype(const QMetaProperty &property, const QCborValue &value, QObject *parent) const override;
	QVariant deserializeSubtype(int propertyType, const QCborValue &value, QObject *parent, const QByteArray &traceHint) const override;

	bool json = false;
	QVariantHash properties;
	struct SerInfo {
		int typeId;
		QVariant variant;
		QCborValue cbor;
	};
	mutable QList<SerInfo> serData;
	mutable QList<SerInfo> deserData;
	QObject *expectedParent = nullptr;
};

Q_DECLARE_METATYPE(QList<DummySerializationHelper::SerInfo>)

#endif // DUMMYSERIALIZATIONHELPER_H
