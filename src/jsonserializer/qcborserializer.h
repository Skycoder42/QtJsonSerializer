#ifndef QCBORSERIALIZER_H
#define QCBORSERIALIZER_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include "QtJsonSerializer/qjsonserializerbase.h"

class QCborSerializerPrivate;
class Q_JSONSERIALIZER_EXPORT QCborSerializer : public QJsonSerializerBase
{
	Q_OBJECT

public:
	enum QCborExtendedTags : std::underlying_type_t<QCborTag> {
		GenericObject = 27,
		RationaleNumber = 30,
		Identifier = 39,
		Homogeneous = 41,
		Set = 258,
		ExplicitMap = 259,
		NetworkAddress = 260,
		NetworkAddressPrefix = 261,
	};
	Q_ENUM(QCborExtendedTags)

	enum QCborCustomTags : std::underlying_type_t<QCborTag> {
		Color = 10000,
		Font = 10001,
		Enum = 10002,
		Flags = 10003,
		ConstructedObject = 10004,
		Pair = 10005,
		MultiMap = 10006,
		VersionNumber = 10007,

		ChronoNanoSeconds = 10100,
		ChronoMicroSeconds = 10101,
		ChronoMilliSeconds = 10102,
		ChronoSeconds = 10103,
		ChronoMinutes = 10104,
		ChronoHours = 10105,

		NoTag = std::numeric_limits<std::underlying_type_t<QCborTag>>::max()
	};
	Q_ENUM(QCborCustomTags)

	explicit QCborSerializer(QObject *parent = nullptr);

	void setTypeTag(int metaTypeId, QCborTag tag = static_cast<QCborTag>(NoTag));
	template <typename T>
	void setTypeTag(QCborTag tag = static_cast<QCborTag>(NoTag));
	QCborTag typeTag(int metaTypeId) const override;
	template <typename T>
	QCborTag typeTag() const;

	//! Serializers a QVariant value to a QCborValue
	QCborValue serialize(const QVariant &data) const;
	//! Serializers a QVariant value to a device
	void serializeTo(QIODevice *device, const QVariant &data, QCborValue::EncodingOptions options = QCborValue::NoTransformation) const;
	//! Serializers a QVariant value to a byte array
	QByteArray serializeTo(const QVariant &data, QCborValue::EncodingOptions options = QCborValue::NoTransformation) const;

	//! Serializers a QObject, Q_GADGET or a list of one of those to cbor
	template <typename T>
	typename _qjsonserializer_helpertypes::json_type<T>::cborType serialize(const T &data) const;
	//! Serializers a QObject, Q_GADGET or a list of one of those to a device
	template <typename T>
	void serializeTo(QIODevice *device, const T &data) const;
	//! Serializers a QQObject, Q_GADGET or a list of one of those to a byte array
	template <typename T>
	QByteArray serializeTo(const T &data) const;

	//! Deserializes a QCborValue to a QVariant value, based on the given type id
	QVariant deserialize(const QCborValue &json, int metaTypeId, QObject *parent = nullptr) const;
	//! Deserializes data from a device to a QVariant value, based on the given type id
	QVariant deserializeFrom(QIODevice *device, int metaTypeId, QObject *parent = nullptr) const;
	//! Deserializes data from a device to a QVariant value, based on the given type id
	QVariant deserializeFrom(const QByteArray &data, int metaTypeId, QObject *parent = nullptr) const;

	//! Deserializes cbor to the given QObject type, Q_GADGET type or a list of one of those types
	template <typename T>
	T deserialize(const typename _qjsonserializer_helpertypes::json_type<T>::cborType &json, QObject *parent = nullptr) const;
	//! Deserializes data from a device to the given QObject type, Q_GADGET type or a list of one of those types
	template <typename T>
	T deserializeFrom(QIODevice *device, QObject *parent = nullptr) const;
	//! Deserializes data from a byte array to the given QObject type, Q_GADGET type or a list of one of those types
	template <typename T>
	T deserializeFrom(const QByteArray &data, QObject *parent = nullptr) const;

	std::variant<QCborValue, QJsonValue> serializeGeneric(const QVariant &value) const override;
	QVariant deserializeGeneric(const std::variant<QCborValue, QJsonValue> &value, int metaTypeId, QObject *parent) const override;

protected:
	// protected implementation -> internal use for the type converters
	bool jsonMode() const override;
	QList<int> typesForTag(QCborTag tag) const override;

private:
	Q_DECLARE_PRIVATE(QCborSerializer)
};

// ------------- generic implementation -------------

template<typename T>
void QCborSerializer::setTypeTag(QCborTag tag)
{
	setTypeTag(qMetaTypeId<T>(), tag);
}

template<typename T>
QCborTag QCborSerializer::typeTag() const
{
	return typeTag(qMetaTypeId<T>());
}

template<typename T>
typename _qjsonserializer_helpertypes::json_type<T>::cborType QCborSerializer::serialize(const T &data) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be serialized");
	return _qjsonserializer_helpertypes::json_type<T>::convert(serialize(_qjsonserializer_helpertypes::variant_helper<T>::toVariant(data)));
}

template<typename T>
void QCborSerializer::serializeTo(QIODevice *device, const T &data) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be serialized");
	serializeTo(device, _qjsonserializer_helpertypes::variant_helper<T>::toVariant(data));
}

template<typename T>
QByteArray QCborSerializer::serializeTo(const T &data) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be serialized");
	return serializeTo(_qjsonserializer_helpertypes::variant_helper<T>::toVariant(data));
}

template<typename T>
T QCborSerializer::deserialize(const typename _qjsonserializer_helpertypes::json_type<T>::cborType &json, QObject *parent) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be deserialized");
	return _qjsonserializer_helpertypes::variant_helper<T>::fromVariant(deserialize(json, qMetaTypeId<T>(), parent));
}

template<typename T>
T QCborSerializer::deserializeFrom(QIODevice *device, QObject *parent) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be deserialized");
	return _qjsonserializer_helpertypes::variant_helper<T>::fromVariant(deserializeFrom(device, qMetaTypeId<T>(), parent));
}

template<typename T>
T QCborSerializer::deserializeFrom(const QByteArray &data, QObject *parent) const
{
	static_assert(_qjsonserializer_helpertypes::is_serializable<T>::value, "T cannot be deserialized");
	return _qjsonserializer_helpertypes::variant_helper<T>::fromVariant(deserializeFrom(data, qMetaTypeId<T>(), parent));
}

#endif // QCBORSERIALIZER_H
