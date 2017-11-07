#ifndef QJSONGEOMCONVERTER_P_H
#define QJSONGEOMCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

class Q_JSONSERIALIZER_EXPORT QJsonSizeConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;
};

class Q_JSONSERIALIZER_EXPORT QJsonPointConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;
};

class Q_JSONSERIALIZER_EXPORT QJsonLineConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;
};

class Q_JSONSERIALIZER_EXPORT QJsonRectConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;
};

#endif // QJSONGEOMCONVERTER_P_H
