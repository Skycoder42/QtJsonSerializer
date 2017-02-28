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

//! A class to serializer and deserializer QObjects and Q_GADGETS to and from JSON
class Q_JSONSERIALIZER_EXPORT QJsonSerializer : public QObject
{
	Q_OBJECT
	friend class QJsonSerializerPrivate;

	//! Specifies, whether null for value types is allowed or not
	Q_PROPERTY(bool allowDefaultNull READ allowDefaultNull WRITE setAllowDefaultNull)
	//! Specifies, whether the `objectName` property of QObjects should be serialized
	Q_PROPERTY(bool keepObjectName READ keepObjectName WRITE setKeepObjectName)

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

	//! Serializers a QVariant value to a QJsonValue
	inline QJsonValue serialize(const QVariant &value) const;
	//! Serializers a QObject to a QJsonObject
	template <typename T>
	inline QJsonObject serialize(T *data) const;
	//! Serializers a Q_GADGET to a QJsonObject
	template <typename T>
	inline QJsonObject serialize(const T &data) const;
	//! Serializers a list of QObjects to a QJsonArray
	template <typename T>
	inline QJsonArray serialize(const QList<T*> &data) const;
	//! Serializers a list of Q_GADGETs to a QJsonArray
	template <typename T>
	inline QJsonArray serialize(const QList<T> &data) const;

	//! Deserializes a QJsonValue to a QVariant value, based on the given type id
	inline QVariant deserialize(const QJsonValue &value, int metaTypeId);
	//! Deserializes a QJsonObject to the given QObject type
	template<typename T>
	inline T *deserialize(QJsonObject jsonObject, QObject *parent) const;
	//! Deserializes a QJsonObject to the given Q_GADGET type
	template <typename T>
	inline T deserialize(QJsonObject jsonObject) const;
	//! Deserializes a QJsonArray to a list of the given QObject type
	template<typename T>
	inline QList<T*> deserialize(QJsonArray jsonArray, QObject *parent) const;
	//! Deserializes a QJsonArray to a list of the given Q_GADGET type
	template<typename T>
	inline QList<T> deserialize(QJsonArray jsonArray) const;

public Q_SLOTS:
	//! @writeAcFn{QJsonSerializer::allowDefaultNull}
	void setAllowDefaultNull(bool allowDefaultNull);
	//! @writeAcFn{QJsonSerializer::keepObjectName}
	void setKeepObjectName(bool keepObjectName);

protected:
	//! Performs the serialization of any QVariant to a json representation
	virtual QJsonValue serializeVariant(int propertyType, const QVariant &value) const;
	//! Performs the serialization of a QObject class to a json object
	virtual QJsonObject serializeObject(const QObject *object) const;
	//! Performs the serialization of a Q_GADGET class to a json object
	virtual QJsonObject serializeGadget(const void *gadget, const QMetaObject *metaObject) const;
	//! Performs the serialization of any QList to a json array
	virtual QJsonArray serializeList(int listType, const QVariantList &value) const;
	//! Performs the serialization of a value type QVariant to a json representation
	virtual QJsonValue serializeValue(int propertyType, QVariant value) const;

	//! Performs the deserialization of any json to a QVariant of the given type
	virtual QVariant deserializeVariant(int propertyType, const QJsonValue &value, QObject *parent) const;
	//! Performs the deserialization of a json object to the given QObject type
	virtual QObject *deserializeObject(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent) const;
	//! Performs the deserialization of a json object to the given Q_GADGET type
	virtual void deserializeGadget(QJsonObject jsonObject, int typeId, void *gadgetPtr) const;
	//! Performs the deserialization of any json array to a list
	virtual QVariantList deserializeList(int listType, const QJsonArray &array, QObject *parent) const;
	//! Performs the deserialization of a json value to a variant value type
	virtual QVariant deserializeValue(int propertyType, QJsonValue value) const;

private:
	bool _allowNull;
	bool _keepObjectName;
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

	auto ok3 = QMetaType::registerComparators<QList<T>>();

	return ok1 && ok2 && ok3;
}

QJsonValue QJsonSerializer::serialize(const QVariant &value) const
{
	return serializeVariant(value.userType(), value);
}

template<typename T>
QJsonObject QJsonSerializer::serialize(T *data) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	Q_UNUSED(T::staticMetaObject);
	return serializeVariant(qMetaTypeId<T*>(), QVariant::fromValue(data)).toObject();
}

template<typename T>
QJsonObject QJsonSerializer::serialize(const T &data) const
{
	Q_UNUSED(T::staticMetaObject);
	return serializeVariant(qMetaTypeId<T>(), QVariant::fromValue(data)).toObject();
}

template<typename T>
QJsonArray QJsonSerializer::serialize(const QList<T*> &data) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	Q_UNUSED(T::staticMetaObject);
	return serializeVariant(qMetaTypeId<QList<T*>>(), QVariant::fromValue(data)).toArray();
}

template<typename T>
QJsonArray QJsonSerializer::serialize(const QList<T> &data) const
{
	Q_UNUSED(T::staticMetaObject);
	return serializeVariant(qMetaTypeId<QList<T>>(), QVariant::fromValue(data)).toArray();
}

QVariant QJsonSerializer::deserialize(const QJsonValue &value, int metaTypeId)
{
	return deserializeVariant(metaTypeId, value, nullptr);
}

template<typename T>
T *QJsonSerializer::deserialize(QJsonObject jsonObject, QObject *parent) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	Q_UNUSED(T::staticMetaObject);
	return deserializeVariant(qMetaTypeId<T*>(), jsonObject, parent).template value<T*>();
}

template<typename T>
T QJsonSerializer::deserialize(QJsonObject jsonObject) const
{
	Q_UNUSED(T::staticMetaObject);
	return deserializeVariant(qMetaTypeId<T>(), jsonObject, nullptr).template value<T>();
}

template<typename T>
QList<T*> QJsonSerializer::deserialize(QJsonArray jsonArray, QObject *parent) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	Q_UNUSED(T::staticMetaObject);
	return deserializeVariant(qMetaTypeId<QList<T*>>(), jsonArray, parent).template value<QList<T*>>();
}

template<typename T>
QList<T> QJsonSerializer::deserialize(QJsonArray jsonArray) const
{
	Q_UNUSED(T::staticMetaObject);
	return deserializeVariant(qMetaTypeId<QList<T>>(), jsonArray, nullptr).template value<QList<T>>();
}

#endif // QJSONSERIALIZER_H
