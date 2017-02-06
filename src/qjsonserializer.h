#ifndef QJSONSERIALIZER_H
#define QJSONSERIALIZER_H

#include "serializerexception.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QMetaProperty>
#include <QObject>
#include <type_traits>

class QJsonSerializer : public QObject
{
	Q_OBJECT
	friend class QJsonSerializerPrivate;

	Q_PROPERTY(bool allowDefaultNull READ allowDefaultNull WRITE setAllowDefaultNull)

public:
	explicit QJsonSerializer(QObject *parent = nullptr);
	~QJsonSerializer();

	bool allowDefaultNull() const;

	inline QJsonValue serialize(const QVariant &value) const;
	inline QJsonObject serialize(const QObject *object) const;
	template<typename T>
	inline QJsonObject serialize(const T &gadget) const;
	template<typename T>
	inline QJsonArray serialize(const QList<T*> &objects) const;
	template<typename T>
	inline QJsonArray serialize(const QList<T> &gadgets) const;

	inline QVariant deserialize(const QJsonValue &value, int metaTypeId);
	inline QObject *deserializeObj(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent = nullptr) const;
	template<typename T>
	inline T *deserializeObj(QJsonObject jsonObject, QObject *parent = nullptr) const;
	template <typename T>
	inline T deserializeGad(QJsonObject jsonObject) const;
	inline QList<QObject*> deserializeObj(QJsonArray jsonArray, const QMetaObject *metaObject, QObject *parent = nullptr) const;
	template<typename T>
	inline QList<T*> deserializeObj(QJsonArray jsonArray, QObject *parent = nullptr) const;
	template<typename T>
	inline QList<T> deserializeGad(QJsonArray jsonArray) const;

public slots:
	void setAllowDefaultNull(bool allowDefaultNull);

protected:
	virtual QJsonObject serializeObject(const QObject *object) const;
	virtual QJsonObject serializeGadget(const void *gadget, const QMetaObject *metaObject) const;
	virtual QJsonValue serializeProperty(int propertyType, const QVariant &value) const;
	virtual QJsonArray serializeList(int listType, const QVariantList &value) const;
	virtual QJsonValue serializeValue(QVariant value) const;

	virtual QObject *deserializeObject(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent) const;
	virtual void deserializeGadget(QJsonObject jsonObject, int typeId, void *gadgetPtr) const;
	virtual QVariant deserializeProperty(int propertyType, const QJsonValue &value, QObject *parent) const;
	virtual QVariantList deserializeList(int listType, const QJsonArray &array, QObject *parent) const;
	virtual QVariant deserializeValue(QJsonValue value) const;

private:
	bool _allowNull;
};

// ------------- Generic Implementation -------------

QJsonValue QJsonSerializer::serialize(const QVariant &value) const
{
	return serializeProperty(value.userType(), value);
}

QJsonObject QJsonSerializer::serialize(const QObject *object) const
{
	return serializeObject(object);
}

template<typename T>
QJsonObject QJsonSerializer::serialize(const T &gadget) const
{
	return serializeGadget(&gadget, &T::staticMetaObject);
}

template<typename T>
QJsonArray QJsonSerializer::serialize(const QList<T *> &objects) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	QJsonArray array;
	foreach(auto object, objects)
		array.append(serializeObject(object));
	return array;
}

template<typename T>
QJsonArray QJsonSerializer::serialize(const QList<T> &gadgets) const
{
	QJsonArray array;
	foreach(auto gadget, gadgets)
		array.append(serializeGadget(&gadget, &T::staticMetaObject));
	return array;
}

QVariant QJsonSerializer::deserialize(const QJsonValue &value, int metaTypeId)
{
	return deserializeProperty(metaTypeId, value, nullptr);
}

QObject *QJsonSerializer::deserializeObj(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent) const
{
	return deserializeObject(jsonObject, metaObject, parent);
}

template<typename T>
T *QJsonSerializer::deserializeObj(QJsonObject jsonObject, QObject *parent) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	return static_cast<T*>(deserializeObject(jsonObject, &T::staticMetaObject, parent));
}

template<typename T>
T QJsonSerializer::deserializeGad(QJsonObject jsonObject) const
{
	auto mId = qMetaTypeId<T>();
	QVariant gadget(mId, nullptr);
	deserializeGadget(jsonObject, mId, gadget.data());
	return gadget.value<T>();
}

QList<QObject *> QJsonSerializer::deserializeObj(QJsonArray jsonArray, const QMetaObject *metaObject, QObject *parent) const
{
	QList<QObject*> list;
	foreach(auto json, jsonArray) {
		if(json.isObject())
			list.append(deserializeObject(json.toObject(), metaObject, parent));
		else {
			throw DeserializationException(QStringLiteral("Failed convert array element of type %1 to %2")
										   .arg(json.type())
										   .arg(metaObject->className()));
		}
	}
	return list;
}

template<typename T>
QList<T*> QJsonSerializer::deserializeObj(QJsonArray jsonArray, QObject *parent) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	QList<T*> list;
	foreach(auto json, jsonArray) {
		if(json.isObject())
			list.append(deserializeObject(json.toObject(), &T::staticMetaObject, parent));
		else {
			throw DeserializationException(QStringLiteral("Failed convert array element of type %1 to %2")
										   .arg(json.type())
										   .arg(T::staticMetaObject.className()));
		}
	}
	return list;
}

template<typename T>
QList<T> QJsonSerializer::deserializeGad(QJsonArray jsonArray) const
{
	auto mId = qMetaTypeId<T>();
	QList<T*> list;
	foreach(auto json, jsonArray) {
		if(json.isObject()) {
			QVariant gadget(mId, nullptr);
			deserializeGadget(json.toObject(), mId, gadget.data());
			list.append(gadget.value<T>());
		}
		else {
			throw DeserializationException(QStringLiteral("Failed convert array element of type %1 to %2")
										   .arg(json.type())
										   .arg(T::staticMetaObject.className()));
		}
	}
	return list;
}

#endif // QJSONSERIALIZER_H
