#ifndef QJSONOBJECTCONVERTER_P_H
#define QJSONOBJECTCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

class Q_JSONSERIALIZER_EXPORT QJsonObjectConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> allowedCborTags(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	int guessType(QCborTag tag, QCborValue::Type dataType) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;

private:
	static const QRegularExpression sharedTypeRegex;
	static const QRegularExpression trackingTypeRegex;

	template<typename T>
	T extract(QVariant variant) const;
	const QMetaObject *getMetaObject(int typeId) const;
	QVariant toVariant(QObject *object, QMetaType::TypeFlags flags) const;
	bool polyMetaObject(QObject *object) const;

	QObject *deserializeGenericObject(const QCborArray &value, QObject *parent) const;
	QObject *deserializeConstructedObject(const QCborValue &value, QObject *parent) const;
	void deserializeProperties(const QMetaObject *metaObject, QObject *object, const QCborMap &value, bool isPoly = false) const;
};

#endif // QJSONOBJECTCONVERTER_P_H
