#ifndef QTJSONSERIALIZER_CBORSERIALIZER_H
#define QTJSONSERIALIZER_CBORSERIALIZER_H

#include "QtJsonSerializer/qtjsonserializer_global.h"
#include "QtJsonSerializer/serializerbase.h"

namespace QtJsonSerializer {

class CborSerializerPrivate;
//! A class to serialize and deserialize c++ classes to and from CBOR
class Q_JSONSERIALIZER_EXPORT CborSerializer : public SerializerBase
{
	Q_OBJECT

	//! If enabled, specially tagged number types will be automatically deserialized to their type
	Q_PROPERTY(bool handleSpecialNumbers READ handleSpecialNumbers WRITE setHandleSpecialNumbers NOTIFY handleSpecialNumbersChanged)

public:
	//! Additional official CBOR-Tags, taken from https://www.iana.org/assignments/cbor-tags/cbor-tags.xhtml
	enum ExtendedTags : std::underlying_type_t<QCborTag> {
		GenericObject = 27, //!< Serialised language-independent object with type name and constructor arguments
		RationaleNumber = 30, //!< Rational number
		Identifier = 39, //!< Identifier
		Homogeneous = 41, //!< Homogeneous Array
		Set = 258, //!< Mathematical finite set
		ExplicitMap = 259, //!< Map datatype with key-value operations (e.g. `.get()/.set()/.delete()`)
		NetworkAddress = 260, //!< Network Address (IPv4 or IPv6 or MAC Address)
		NetworkAddressPrefix = 261, //!< Network Address Prefix (IPv4 or IPv6 Address + Mask Length)
	};
	Q_ENUM(ExtendedTags)

	// Additional unofficial CBOR-Tags, as defined by this library to tag Qt types for better typesafety
	enum CustomTags : std::underlying_type_t<QCborTag> {
		Color = 10000, //!< Tag used for QColor
		Font = 10001, //!< Tag used for QFont
		Enum = 10002, //!< Tag used for enums
		Flags = 10003, //!< Tag used for flags
		ConstructedObject = 10004, //!< Tag used for constructed object (GenericObject + standard object)
		Pair = 10005, //!< Tag used for QPair/std::pair
		MultiMap = 10006, //!< Tag used for QMultiMap/QMultiHash
		VersionNumber = 10007, //!< Tag used for QVersionNumber
		Tuple = 10008, //!< Tag used for std::tuple
		BitArray = 10009, //!< Tag used for QBitArray
		Date = 10010, //!< Tag used for QDate (short ISO format)
		Time = 10011, //!< Tag used for QTime (short ISO format)

		LocaleISO = 10100, //!< Tag used for QLocale, encoded via the ISO format
		LocaleBCP47 = 10101, //!< Tag used for QLocale, encoded via the BCP47 format

		GeomSize = 10110, //!< Tag used for QSize/QSizeF
		GeomPoint = 10111, //!< Tag used for QPoint/QPointF
		GeomLine = 10112, //!< Tag used for QLine/QLineF
		GeomRect = 10113, //!< Tag used for QRect/QRectF

		ChronoNanoSeconds = 10120, //!< Tag used for std::chrono::nanoseconds
		ChronoMicroSeconds = 10121, //!< Tag used for std::chrono::microseconds
		ChronoMilliSeconds = 10122, //!< Tag used for std::chrono::milliseconds
		ChronoSeconds = 10123, //!< Tag used for std::chrono::seconds
		ChronoMinutes = 10124, //!< Tag used for std::chrono::minutes
		ChronoHours = 10125, //!< Tag used for std::chrono::hours

		NoTag = std::numeric_limits<std::underlying_type_t<QCborTag>>::max() //!< Used as placeholder for setTypeTag to not change the tag
	};
	Q_ENUM(CustomTags)

	//! Default constructor
	explicit CborSerializer(QObject *parent = nullptr);

	//! @readAcFn{CborSerializer::handleSpecialNumbers}
	bool handleSpecialNumbers() const;

	//! Set a tag to always be used when serializing the given type
	template <typename T>
	void setTypeTag(QCborTag tag = static_cast<QCborTag>(NoTag));
	//! @copybrief CborSerializer::setTypeTag(QCborTag)
	void setTypeTag(int metaTypeId, QCborTag tag = static_cast<QCborTag>(NoTag));
	//! @copybrief TypeConverter::SerializationHelper::typeTag
	template <typename T>
	QCborTag typeTag() const;
	QCborTag typeTag(int metaTypeId) const override;

	//! Serializers a QVariant value to a QCborValue
	QCborValue serialize(const QVariant &data) const;
	//! Serializers a QVariant value to a device
	void serializeTo(QIODevice *device, const QVariant &data, QCborValue::EncodingOptions options = QCborValue::NoTransformation) const;
	//! Serializers a QVariant value to a byte array
	QByteArray serializeTo(const QVariant &data, QCborValue::EncodingOptions options = QCborValue::NoTransformation) const;

	//! Serializers a c++ type to cbor
	template <typename T>
	QCborValue serialize(const T &data) const;
	//! Serializers a c++ type to a device
	template <typename T>
	void serializeTo(QIODevice *device, const T &data, QCborValue::EncodingOptions options = QCborValue::NoTransformation) const;
	//! Serializers a c++ type to a byte array
	template <typename T>
	QByteArray serializeTo(const T &data, QCborValue::EncodingOptions options = QCborValue::NoTransformation) const;

	//! Deserializes a QCborValue to a QVariant value, based on the given type id
	QVariant deserialize(const QCborValue &cbor, int metaTypeId, QObject *parent = nullptr) const;
	//! Deserializes data from a device to a QVariant value, based on the given type id
	QVariant deserializeFrom(QIODevice *device, int metaTypeId, QObject *parent = nullptr) const;
	//! Deserializes data from a device to a QVariant value, based on the given type id
	QVariant deserializeFrom(const QByteArray &data, int metaTypeId, QObject *parent = nullptr) const;

	//! Deserializes cbor to the given c++ type
	template <typename T>
	T deserialize(const QCborValue &cbor, QObject *parent = nullptr) const;
	//! Deserializes data from a device to the given c++ type
	template <typename T>
	T deserializeFrom(QIODevice *device, QObject *parent = nullptr) const;
	//! Deserializes data from a byte array to the given c++ type
	template <typename T>
	T deserializeFrom(const QByteArray &data, QObject *parent = nullptr) const;

	std::variant<QCborValue, QJsonValue> serializeGeneric(const QVariant &value) const override;
	QVariant deserializeGeneric(const std::variant<QCborValue, QJsonValue> &value, int metaTypeId, QObject *parent) const override;

public Q_SLOTS:
	//! @writeAcFn{CborSerializer::handleSpecialNumbers}
	void setHandleSpecialNumbers(bool handleSpecialNumbers);

Q_SIGNALS:
	//! @notifyAcFn{CborSerializer::handleSpecialNumbers}
	void handleSpecialNumbersChanged(bool handleSpecialNumbers, QPrivateSignal);

protected:
	// protected implementation -> internal use for the type converters
	bool jsonMode() const override;
	QList<int> typesForTag(QCborTag tag) const override;

private:
	Q_DECLARE_PRIVATE(CborSerializer)
};

// ------------- generic implementation -------------

template<typename T>
void CborSerializer::setTypeTag(QCborTag tag)
{
	setTypeTag(qMetaTypeId<T>(), tag);
}

template<typename T>
QCborTag CborSerializer::typeTag() const
{
	return typeTag(qMetaTypeId<T>());
}

template<typename T>
QCborValue CborSerializer::serialize(const T &data) const
{
	static_assert(__private::is_serializable<T>::value, "T cannot be serialized");
	return serialize(__private::variant_helper<T>::toVariant(data));
}

template<typename T>
void CborSerializer::serializeTo(QIODevice *device, const T &data, QCborValue::EncodingOptions options) const
{
	static_assert(__private::is_serializable<T>::value, "T cannot be serialized");
	serializeTo(device, __private::variant_helper<T>::toVariant(data), options);
}

template<typename T>
QByteArray CborSerializer::serializeTo(const T &data, QCborValue::EncodingOptions options) const
{
	static_assert(__private::is_serializable<T>::value, "T cannot be serialized");
	return serializeTo(__private::variant_helper<T>::toVariant(data), options);
}

template<typename T>
T CborSerializer::deserialize(const QCborValue &cbor, QObject *parent) const
{
	static_assert(__private::is_serializable<T>::value, "T cannot be deserialized");
	return __private::variant_helper<T>::fromVariant(deserialize(cbor, qMetaTypeId<T>(), parent));
}

template<typename T>
T CborSerializer::deserializeFrom(QIODevice *device, QObject *parent) const
{
	static_assert(__private::is_serializable<T>::value, "T cannot be deserialized");
	return __private::variant_helper<T>::fromVariant(deserializeFrom(device, qMetaTypeId<T>(), parent));
}

template<typename T>
T CborSerializer::deserializeFrom(const QByteArray &data, QObject *parent) const
{
	static_assert(__private::is_serializable<T>::value, "T cannot be deserialized");
	return __private::variant_helper<T>::fromVariant(deserializeFrom(data, qMetaTypeId<T>(), parent));
}

}

#endif // QTJSONSERIALIZER_CBORSERIALIZER_H
