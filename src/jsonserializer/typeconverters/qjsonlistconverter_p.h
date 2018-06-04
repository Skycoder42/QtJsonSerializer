#ifndef QJSONLISTCONVERTER_P_H
#define QJSONLISTCONVERTER_P_H

#ifdef NO_QT_MODULE
    #include "qtjsonserializer_global.h"
    #include "qjsontypeconverter.h"
#else
    #include "QtJsonSerializer/qtjsonserializer_global.h"
    #include "QtJsonSerializer/qjsontypeconverter.h"
#endif

#include <QtCore/QRegularExpression>

class Q_JSONSERIALIZER_EXPORT QJsonListConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;

private:
	static const QRegularExpression listTypeRegex;

	static int getSubtype(int listType);
};

#endif // QJSONLISTCONVERTER_P_H
