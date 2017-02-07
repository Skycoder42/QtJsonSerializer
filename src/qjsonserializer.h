#ifndef QJSONSERIALIZER_H
#define QJSONSERIALIZER_H

#include "serializerexception.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QMetaProperty>
#include <QObject>
#include <QVariant>
#include <QDebug>
#include <type_traits>

class QJsonSerializer : public QObject
{
	Q_OBJECT
	friend class QJsonSerializerPrivate;

	Q_PROPERTY(bool allowDefaultNull READ allowDefaultNull WRITE setAllowDefaultNull)
	Q_PROPERTY(bool keepObjectName READ keepObjectName WRITE setKeepObjectName)

public:
	explicit QJsonSerializer(QObject *parent = nullptr);
	~QJsonSerializer();

	template<typename T>
	static bool registerListConverters();

	bool allowDefaultNull() const;
	bool keepObjectName() const;

	inline QJsonValue serialize(const QVariant &value) const;
	template <typename T>
	inline QJsonObject serialize(T *data) const;
	template <typename T>
	inline QJsonObject serialize(const T &data) const;
	template <typename T>
	inline QJsonArray serialize(const QList<T*> &data) const;
	template <typename T>
	inline QJsonArray serialize(const QList<T> &data) const;

	inline QVariant deserialize(const QJsonValue &value, int metaTypeId);
	template<typename T>
	inline T *deserialize(QJsonObject jsonObject, QObject *parent = nullptr) const;
	template <typename T>
	inline T deserialize(QJsonObject jsonObject) const;
	template<typename T>
	inline QList<T*> deserialize(QJsonArray jsonArray, QObject *parent = nullptr) const;
	template<typename T>
	inline QList<T> deserialize(QJsonArray jsonArray) const;

public slots:
	void setAllowDefaultNull(bool allowDefaultNull);
	void setKeepObjectName(bool keepObjectName);

protected:
	virtual QJsonValue serializeVariant(int propertyType, const QVariant &value) const;
	virtual QJsonObject serializeObject(const QObject *object) const;
	virtual QJsonObject serializeGadget(const void *gadget, const QMetaObject *metaObject) const;
	virtual QJsonArray serializeList(int listType, const QVariantList &value) const;
	virtual QJsonValue serializeValue(QVariant value) const;

	virtual QVariant deserializeVariant(int propertyType, const QJsonValue &value, QObject *parent) const;
	virtual QObject *deserializeObject(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent) const;
	virtual void deserializeGadget(QJsonObject jsonObject, int typeId, void *gadgetPtr) const;
	virtual QVariantList deserializeList(int listType, const QJsonArray &array, QObject *parent) const;
	virtual QVariant deserializeValue(QJsonValue value) const;

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
