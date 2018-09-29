#ifndef DUMMYSERIALIZATIONHELPER_H
#define DUMMYSERIALIZATIONHELPER_H

#include <QtCore/QQueue>
#include <QtJsonSerializer/QJsonTypeConverter>

class DummySerializationHelper : public QObject, public QJsonTypeConverter::SerializationHelper
{
	Q_OBJECT

public:
	DummySerializationHelper(QObject *parent = nullptr);

	QVariant getProperty(const char *name) const override;
	QJsonValue serializeSubtype(QMetaProperty property, const QVariant &value) const override;
	QJsonValue serializeSubtype(int propertyType, const QVariant &value, const QByteArray &traceHint) const override;
	QVariant deserializeSubtype(QMetaProperty property, const QJsonValue &value, QObject *parent) const override;
	QVariant deserializeSubtype(int propertyType, const QJsonValue &value, QObject *parent, const QByteArray &traceHint) const override;

	QVariantHash properties;
	struct SerInfo {
		int typeId;
		QVariant variant;
		QJsonValue json;
	};
	mutable QList<SerInfo> serData;
	mutable QList<SerInfo> deserData;
};

Q_DECLARE_METATYPE(QList<DummySerializationHelper::SerInfo>)

#endif // DUMMYSERIALIZATIONHELPER_H
