#ifndef QJSONSERIALIZER_H
#define QJSONSERIALIZER_H

#include "QtJsonSerializer/qjsonserializer_global.h"
#include "QtJsonSerializer/qjsonserializerexception.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <type_traits>

class QJsonSerializerPrivate;
//! A class to serializer and deserializer QObjects and Q_GADGETS to and from JSON
class Q_JSONSERIALIZER_EXPORT QJsonSerializer : public QObject
{
	Q_OBJECT

	//! Specifies, whether null for value types is allowed or not
	Q_PROPERTY(bool allowDefaultNull READ allowDefaultNull WRITE setAllowDefaultNull)
	//! Specifies, whether the `objectName` property of QObjects should be serialized
	Q_PROPERTY(bool keepObjectName READ keepObjectName WRITE setKeepObjectName)
	//! Specifies, whether enums should be serialized as integer or as string
	Q_PROPERTY(bool enumAsString READ enumAsString WRITE setEnumAsString)

public:
	//! Constructor
	explicit QJsonSerializer(QObject *parent = nullptr);
	//! Destructor
	~QJsonSerializer();

	//! Registers a custom type for list converisons
	template<typename T>
	static bool registerListConverters();

	//! @readAcFn{QJsonSerializer::allowDefaultNull}
	bool allowDefaultNull() const;
	//! @readAcFn{QJsonSerializer::keepObjectName}
	bool keepObjectName() const;
	//! @readAcFn{QJsonSerializer::enumAsString}
	bool enumAsString() const;

	//! Serializers a QVariant value to a QJsonValue
	inline QJsonValue serialize(const QVariant &data) const;
	//! Serializers a QObject or Q_GADGET to a QJsonObject
	template <typename T>
	inline QJsonObject serialize(const T &data) const;
	//! Serializers a list of QObjects or Q_GADGETs to a QJsonArray
	template <typename T>
	inline QJsonArray serialize(const QList<T> &data) const;

	//! Deserializes a QJsonValue to a QVariant value, based on the given type id
	inline QVariant deserialize(const QJsonValue &json, int metaTypeId, QObject *parent = nullptr) const;
	//! Deserializes a QJsonObject to the given QObject or Q_GADGET type
	template <typename T>
	inline T deserialize(const QJsonObject &json, QObject *parent = nullptr) const;
	//! Deserializes a QJsonArray to a list of the given QObject or Q_GADGET type
	template<typename T>
	inline QList<T> deserialize(const QJsonArray &json, QObject *parent = nullptr) const;

public Q_SLOTS:
	//! @writeAcFn{QJsonSerializer::allowDefaultNull}
	void setAllowDefaultNull(bool allowDefaultNull);
	//! @writeAcFn{QJsonSerializer::keepObjectName}
	void setKeepObjectName(bool keepObjectName);
	//! @writeAcFn{QJsonSerializer::enumAsString}
	void setEnumAsString(bool enumAsString);

protected:
	//! Performs the serialization of any QVariant to a json representation
	virtual QJsonValue serializeVariant(int propertyType, const QVariant &value) const;
	//! Performs the serialization of a QObject class to a json object
	virtual QJsonObject serializeObject(const QObject *object) const;
	//! Performs the serialization of a Q_GADGET class to a json object
	virtual QJsonObject serializeGadget(const void *gadget, const QMetaObject *metaObject) const;
	//! Performs the serialization of any QList to a json array
	virtual QJsonArray serializeList(int listType, const QVariantList &value) const;
	//! Performs the serialization of an enum type QVariant to a json representation
	virtual QJsonValue serializeEnum(const QMetaEnum &metaEnum, const QVariant &value) const;
	//! Performs the serialization of a value type QVariant to a json representation
	virtual QJsonValue serializeValue(int propertyType, const QVariant &value) const;

	//! Performs the deserialization of any json to a QVariant of the given type
	virtual QVariant deserializeVariant(int propertyType, const QJsonValue &value, QObject *parent) const;
	//! Performs the deserialization of a json object to the given QObject type
	virtual QObject *deserializeObject(const QJsonObject &jsonObject, const QMetaObject *metaObject, QObject *parent) const;
	//! Performs the deserialization of a json object to the given Q_GADGET type
	virtual void deserializeGadget(const QJsonObject &jsonObject, int typeId, void *gadgetPtr) const;
	//! Performs the deserialization of any json array to a list
	virtual QVariantList deserializeList(int listType, const QJsonArray &array, QObject *parent) const;
	//! Performs the deserialization of an enum value to a variant value type
	virtual QVariant deserializeEnum(const QMetaEnum &metaEnum, const QJsonValue &value) const;
	//! Performs the deserialization of a json value to a variant value type
	virtual QVariant deserializeValue(int propertyType, const QJsonValue &value) const;

private:
	template <typename T>
	class _assert_has_metaobject : public std::is_void<typename T::QtGadgetHelper> {};
	template <typename T>
	class _assert_has_metaobject<T*> : public std::is_base_of<QObject, T> {};
	QScopedPointer<QJsonSerializerPrivate> d;
};
// ------------- Generic Implementation -------------

template<typename T>
bool QJsonSerializer::registerListConverters() {
	auto ok1 = QMetaType::registerConverter<QList<T>, QVariantList>([](const QList<T> &list) -> QVariantList {
		QVariantList l;
		foreach(auto v, list)
			l.append(QVariant::fromValue(v));
		return l;
	});

	auto ok2 = QMetaType::registerConverter<QVariantList, QList<T>>([](const QVariantList &list) -> QList<T> {
		QList<T> l;
		foreach(auto v, list) {
			auto vt = v.type();
			if(v.convert(qMetaTypeId<T>()))
				l.append(v.value<T>());
			else {
				qWarning() << QByteArray("Conversion to")
						   << QMetaType::typeName(qMetaTypeId<QList<T>>())
						   << QByteArray("failed, could not convert element of type")
						   << QMetaType::typeName(vt);
				l.append(T());
			}
		}
		return l;
	});

	return ok1 && ok2;
}

QJsonValue QJsonSerializer::serialize(const QVariant &data) const
{
	return serializeVariant(data.userType(), data);
}

template<typename T>
QJsonObject QJsonSerializer::serialize(const T &data) const
{
	static_assert(_assert_has_metaobject<T>::value, "T must either be a pointer and inherit QObject or be a value type and have the Q_GADGET macro");
	return serializeVariant(qMetaTypeId<T>(), QVariant::fromValue(data)).toObject();
}

template<typename T>
QJsonArray QJsonSerializer::serialize(const QList<T> &data) const
{
	static_assert(_assert_has_metaobject<T>::value, "T must either be a pointer and inherit QObject or be a value type and have the Q_GADGET macro");
	return serializeVariant(qMetaTypeId<QList<T>>(), QVariant::fromValue(data)).toArray();
}

QVariant QJsonSerializer::deserialize(const QJsonValue &json, int metaTypeId, QObject *parent) const
{
	return deserializeVariant(metaTypeId, json, parent);
}

template<typename T>
T QJsonSerializer::deserialize(const QJsonObject &json, QObject *parent) const
{
	static_assert(_assert_has_metaobject<T>::value, "T must either be a pointer and inherit QObject or be a value type and have the Q_GADGET macro");
	return deserializeVariant(qMetaTypeId<T>(), json, parent).template value<T>();
}

template<typename T>
QList<T> QJsonSerializer::deserialize(const QJsonArray &json, QObject *parent) const
{
	static_assert(_assert_has_metaobject<T>::value, "T must either be a pointer and inherit QObject or be a value type and have the Q_GADGET macro");
	return deserializeVariant(qMetaTypeId<QList<T>>(), json, parent).template value<QList<T>>();
}

#endif // QJSONSERIALIZER_H
