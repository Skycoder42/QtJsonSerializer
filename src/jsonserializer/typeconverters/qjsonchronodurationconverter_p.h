#ifndef QJSONCHRONODURATIONCONVERTER_P_H
#define QJSONCHRONODURATIONCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"
#include "qjsonserializerexception.h"

#include <chrono>

#include <QDebug>

class Q_JSONSERIALIZER_EXPORT QJsonChronoDurationConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QCborTag> allowedCborTags(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	int guessType(QCborTag tag, QCborValue::Type dataType) const override;
	QCborValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent, const SerializationHelper *helper) const override;

private:
	using MetaDuration = std::variant<std::chrono::nanoseconds,
									  std::chrono::microseconds,
									  std::chrono::milliseconds,
									  std::chrono::seconds,
									  std::chrono::minutes,
									  std::chrono::hours>;

	QCborTag tagForType(int metaTypeId) const;
	MetaDuration parseValue(int propertyType, const QCborValue &value, const SerializationHelper *helper) const;

	template <typename TDuration>
	TDuration create(const QCborValue &value) const {
		return TDuration{static_cast<typename TDuration::rep>((value.isTag() ? value.taggedValue() : value).toInteger())};
	}

	template <typename TDuration>
	TDuration cast(const MetaDuration &duration) const {
		return std::visit([](const auto &dur) {
			if (std::is_convertible_v<std::decay_t<decltype(dur)>, TDuration>)
				return std::chrono::duration_cast<TDuration>(dur);
			else {
				throw QJsonDeserializationException{QByteArray{"Unable to upcast from deserialized "} +
													QMetaType::typeName(qMetaTypeId<std::decay_t<decltype(dur)>>()) +
													QByteArray{" to the requested "} +
													QMetaType::typeName(qMetaTypeId<TDuration>())};
			}
		}, duration);
	}
};

#endif // QJSONCHRONODURATIONCONVERTER_P_H
