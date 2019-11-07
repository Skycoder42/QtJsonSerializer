#ifndef QTJSONSERIALIZER_VERSIONNUMBERCONVERTER_P_H
#define QTJSONSERIALIZER_VERSIONNUMBERCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "typeconverter.h"

#include <QtCore/QLoggingCategory>

namespace QtJsonSerializer::TypeConverters {

class Q_JSONSERIALIZER_EXPORT VersionNumberConverter : public TypeConverter
{
public:
	QT_JSONSERIALIZER_TYPECONVERTER_NAME(VersionNumberConverter)
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> allowedCborTags(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	int guessType(QCborTag tag, QCborValue::Type dataType) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;
};

Q_DECLARE_LOGGING_CATEGORY(logVersionConverter)

}

#endif // QTJSONSERIALIZER_VERSIONNUMBERCONVERTER_P_H
