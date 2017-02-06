#ifndef QJSONSERIALIZER_H
#define QJSONSERIALIZER_H

#include "serializerexception.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QMetaProperty>
#include <QObject>
#include <type_traits>

class QJsonSerializerPrivate;
class QJsonSerializer : public QObject
{
	Q_OBJECT
	friend class QJsonSerializerPrivate;

	Q_PROPERTY(bool allowDefaultNull READ allowDefaultNull WRITE setAllowDefaultNull)

public:
	explicit QJsonSerializer(QObject *parent = nullptr);
	~QJsonSerializer();

	bool allowDefaultNull() const;

	virtual QJsonObject serialize(const QObject *object) const;
	template<typename T>
	QJsonArray serialize(const QList<T*> objects) const;

	virtual QObject *deserialize(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent = nullptr) const;
	template<typename T>
	T *deserialize(QJsonObject jsonObject, QObject *parent = nullptr) const;
	QList<QObject*> deserialize(QJsonArray jsonArray, const QMetaObject *metaObject, QObject *parent = nullptr) const;
	template<typename T>
	QList<T*> deserialize(QJsonArray jsonArray, QObject *parent = nullptr) const;

public slots:
	void setAllowDefaultNull(bool allowDefaultNull);

protected:
	virtual QJsonValue serializeValue(QVariant value);
	virtual QVariant deserializeValue(QJsonValue value);

private:
	QScopedPointer<QJsonSerializerPrivate> d_ptr;
};

// ------------- Generic Implementation -------------

template<typename T>
QJsonArray QJsonSerializer::serialize(const QList<T*> objects) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	QJsonArray array;
	foreach(auto obj, objects)
		array.append(serialize(obj));
	return array;
}

template<typename T>
T *QJsonSerializer::deserialize(QJsonObject jsonObject, QObject *parent) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	return static_cast<T*>(deserialize(jsonObject, &T::staticMetaObject, parent));
}

template<typename T>
QList<T*> QJsonSerializer::deserialize(QJsonArray jsonArray, QObject *parent) const
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	QList<T*> list;
	foreach(auto json, jsonArray) {
		if(json.isObject())
			list.append(deserialize<T>(json.toObject(), parent));
		else {
			throw DeserializationException(QStringLiteral("Failed convert array element of type %1 to %2")
										   .arg(json.type())
										   .arg(T::staticMetaObject.className()));
		}
	}
	return list;
}

#endif // QJSONSERIALIZER_H
