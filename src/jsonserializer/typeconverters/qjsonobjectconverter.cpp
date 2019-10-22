#include "qjsonobjectconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"
#include "qjsonserializerbase_p.h"

#include <array>

#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>

const QRegularExpression QJsonObjectConverter::sharedTypeRegex(QStringLiteral(R"__(^QSharedPointer<\s*(.*?)\s*>$)__"));
const QRegularExpression QJsonObjectConverter::trackingTypeRegex(QStringLiteral(R"__(^QPointer<\s*(.*?)\s*>$)__"));

bool QJsonObjectConverter::canConvert(int metaTypeId) const
{
	auto flags = QMetaType::typeFlags(metaTypeId);
	return flags.testFlag(QMetaType::PointerToQObject) ||
			flags.testFlag(QMetaType::SharedPointerToQObject) ||  // weak ptr cannot be constructed
			flags.testFlag(QMetaType::TrackingPointerToQObject);
}

QList<QCborTag> QJsonObjectConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {
		NoTag,
		static_cast<QCborTag>(QCborSerializer::GenericObject),
		static_cast<QCborTag>(QCborSerializer::ConstructedObject)
	};
}

QList<QCborValue::Type> QJsonObjectConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	switch (static_cast<quint64>(tag)) {
	case QCborSerializer::GenericObject:
		return {QCborValue::Array};
	case QCborSerializer::ConstructedObject:
		return {QCborValue::Array, QCborValue::Null};
	default:
		return {QCborValue::Map, QCborValue::Null};
	}
}

int QJsonObjectConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	Q_UNUSED(dataType)
	switch (static_cast<quint64>(tag)) {
	case QCborSerializer::GenericObject:
	case QCborSerializer::ConstructedObject:
		return QMetaType::QObjectStar;
	default:
		return QMetaType::UnknownType;
	}
}

QCborValue QJsonObjectConverter::serialize(int propertyType, const QVariant &value) const
{
	QObject *object = nullptr;
	auto flags = QMetaType::typeFlags(propertyType);
	if (flags.testFlag(QMetaType::PointerToQObject))
		object = extract<QObject*>(value);
	else if (flags.testFlag(QMetaType::SharedPointerToQObject))
		object = extract<QSharedPointer<QObject>>(value).data();
	else if (flags.testFlag(QMetaType::TrackingPointerToQObject))
		object = extract<QPointer<QObject>>(value).data();
	else
		Q_UNREACHABLE();

	if (!object)
		return QCborValue::Null;

	//get the metaobject, based on polymorphism
	const QMetaObject *meta = nullptr;
	auto poly = static_cast<QJsonSerializerBase::Polymorphing>(helper()->getProperty("polymorphing").toInt());
	auto isPoly = false;
	switch (poly) {
	case QJsonSerializerBase::Polymorphing::Disabled:
		isPoly = false;
		break;
	case QJsonSerializerBase::Polymorphing::Enabled:
		isPoly = polyMetaObject(object);
		break;
	case QJsonSerializerBase::Polymorphing::Forced:
		isPoly = true;
		break;
	default:
		Q_UNREACHABLE();
		break;
	}

	QCborMap cborMap;

	if(isPoly) {
		meta = object->metaObject();
		//first: pass the class name
		cborMap[QStringLiteral("@class")] = QString::fromUtf8(meta->className());
	} else
		meta = getMetaObject(propertyType);

	//go through all properties and try to serialize them
	const auto keepObjectName = helper()->getProperty("keepObjectName").toBool();
	const auto ignoreStoredAttribute = helper()->getProperty("ignoreStoredAttribute").toBool();
	auto i = QObject::staticMetaObject.indexOfProperty("objectName");
	if (!keepObjectName)
		i++;
	for(; i < meta->propertyCount(); i++) {
		auto property = meta->property(i);
		if (ignoreStoredAttribute || property.isStored())
			cborMap[QString::fromUtf8(property.name())] = helper()->serializeSubtype(property, property.read(object));
	}

	return cborMap;
}

QVariant QJsonObjectConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	if ((value.isTag() ? value.taggedValue() : value).isNull())
		return toVariant(nullptr, QMetaType::typeFlags(propertyType));

	QCborMap cborMap;
	if (value.isTag()) {
		if (value.tag() == static_cast<QCborTag>(QCborSerializer::GenericObject))
			return toVariant(deserializeGenericObject(value.taggedValue().toArray(), parent), QMetaType::typeFlags(propertyType));
		else if (value.tag() == static_cast<QCborTag>(QCborSerializer::ConstructedObject))
			return toVariant(deserializeConstructedObject(value.taggedValue(), parent), QMetaType::typeFlags(propertyType));
		else
			cborMap = value.taggedValue().toMap();
	} else
		cborMap = value.toMap();

	auto poly = static_cast<QJsonSerializerBase::Polymorphing>(helper()->getProperty("polymorphing").toInt());

	auto metaObject = getMetaObject(propertyType);
	if (!metaObject)
		throw QJsonDeserializationException(QByteArray("Unable to get metaobject for type ") + QMetaType::typeName(propertyType));

	// try to get the polymorphic metatype (if allowed)
	auto isPoly = false;
	if (poly != QJsonSerializerBase::Polymorphing::Disabled) {
		if (cborMap.contains(QStringLiteral("@class"))) {
			isPoly = true;
			QByteArray classField = cborMap[QStringLiteral("@class")].toString().toUtf8() + "*";  // add the star
			auto typeId = QMetaType::type(classField.constData());
			auto nMeta = QMetaType::metaObjectForType(typeId);
			if (!nMeta)
				throw QJsonDeserializationException("Unable to find class requested from json \"@class\" property: " + classField);
			if (!nMeta->inherits(metaObject)) {
				throw QJsonDeserializationException("Requested class from \"@class\" field, " +
													classField +
													QByteArray(", does not inhert the property type ") +
													QMetaType::typeName(propertyType));
			}
			metaObject = nMeta;
		} else if (poly == QJsonSerializerBase::Polymorphing::Forced)
			throw QJsonDeserializationException("Json does not contain the \"@class\" field, but forced polymorphism requires it");
	}

	// try to construct the object
	auto object = metaObject->newInstance(Q_ARG(QObject*, parent));
	if (!object) {
		throw QJsonDeserializationException(QByteArray("Failed to construct object of type ") +
											metaObject->className() +
											QByteArray(" (Does the constructor \"Q_INVOKABLE class(QObject*);\" exist?)"));
	}

	deserializeProperties(metaObject, object, cborMap, isPoly);
	return toVariant(object, QMetaType::typeFlags(propertyType));
}

const QMetaObject *QJsonObjectConverter::getMetaObject(int typeId) const
{
	auto flags = QMetaType::typeFlags(typeId);
	if (flags.testFlag(QMetaType::PointerToQObject))
		return QMetaType::metaObjectForType(typeId);
	else {
		QRegularExpression regex;
		if (flags.testFlag(QMetaType::SharedPointerToQObject))
			regex = sharedTypeRegex;
		else if (flags.testFlag(QMetaType::TrackingPointerToQObject))
			regex = trackingTypeRegex;
		else {
			Q_UNREACHABLE();
			return nullptr;
		}

		//extract template type, and if found, add the pointer star and find the meta type
		auto match = regex.match(QString::fromUtf8(helper()->getCanonicalTypeName(typeId)));
		if (match.hasMatch()) {
			auto type = QMetaType::type(match.captured(1).toUtf8().trimmed() + "*");
			return QMetaType::metaObjectForType(type);
		} else
			return nullptr;
	}
}

template<typename T>
T QJsonObjectConverter::extract(QVariant variant) const
{
	auto id = qMetaTypeId<T>();
	if (variant.canConvert(id) && variant.convert(id))
		return variant.value<T>();
	else {
		throw QJsonSerializationException(QByteArray("unable to get QObject pointer from type ") +
										  QMetaType::typeName(id) +
										  QByteArray(". Make shure to register pointer converters via QJsonSerializer::registerPointerConverters"));
	}
}

QVariant QJsonObjectConverter::toVariant(QObject *object, QMetaType::TypeFlags flags) const
{
	if (flags.testFlag(QMetaType::PointerToQObject))
		return QVariant::fromValue(object);
	else if (flags.testFlag(QMetaType::SharedPointerToQObject)) {
		//remove parent, as shared pointers and object tree exclude each other
		if (object)
			object->setParent(nullptr);
		return QVariant::fromValue(QSharedPointer<QObject>(object));
	} else if (flags.testFlag(QMetaType::TrackingPointerToQObject))
		return QVariant::fromValue<QPointer<QObject>>(object);
	else {
		Q_UNREACHABLE();
		return QVariant();
	}
}

bool QJsonObjectConverter::polyMetaObject(QObject *object) const
{
	auto meta = object->metaObject();

	//check the internal property
	if (object->dynamicPropertyNames().contains("__qt_json_serializer_polymorphic")) {
		auto polyProp = object->property("__qt_json_serializer_polymorphic").toBool();
		return polyProp;
	}

	//check the class info
	auto polyIndex = meta->indexOfClassInfo("polymorphic");
	if (polyIndex != -1) {
		auto info = meta->classInfo(polyIndex);
		if (info.value() == QByteArray("true"))
			return true;// use the object
		else if (info.value() == QByteArray("false"))
			return false;// use the class
		else
			qWarning() << "Invalid value for polymorphic classinfo on object type" << meta->className() << "ignored";
	}

	//default: the class
	return false;// use the class
}

QObject *QJsonObjectConverter::deserializeGenericObject(const QCborArray &value, QObject *parent) const
{
	if (value.size() == 0)
		throw QJsonDeserializationException{"A GenericObject requires at least one argument, specifying the object name"};

	// find a meta object
	QByteArray className = value.first().toString().toUtf8() + "*";  // add the star
	auto typeId = QMetaType::type(className.constData());
	auto metaObject = QMetaType::metaObjectForType(typeId);
	if (!metaObject)
		throw QJsonDeserializationException("Unable to find class requested from GenericObject tagged array: " + className);

	// deserialize all arguments
	QVariantList arguments;
	arguments.reserve(static_cast<int>(value.size() - 1));
	for (auto aIdx = 1ll; aIdx < value.size(); ++aIdx)
		arguments.append(helper()->deserializeSubtype(QMetaType::UnknownType, value[aIdx], nullptr, className + "[" + QByteArray::number(aIdx - 1) + "]"));

	// find a matching constructor
	for (auto cIdx = 0; cIdx < metaObject->constructorCount(); ++cIdx) {
		const auto constructor = metaObject->constructor(cIdx);
		// verify same argument count (but allow extra QObject argument for parenting)
		auto extraObj = false;
		if (constructor.parameterCount() != arguments.size()) {
			if (constructor.parameterCount() == arguments.size() + 1 &&
				constructor.parameterType(constructor.parameterCount() - 1) == QMetaType::QObjectStar)
				extraObj = true;
			else
				continue;
		}

		// verify each argument can be converted (by converting it)
		auto argCopy = arguments;
		auto allOk = true;
		for (auto pIdx = 0; pIdx < argCopy.size(); ++pIdx) {
			const auto pType = constructor.parameterType(pIdx);
			if (!argCopy[pIdx].canConvert(pType) || !argCopy[pIdx].convert(pType)) {
				allOk = false;
				break;
			}
		}

		// if all arguments could be converted -> construct the object
		if (allOk) {
			if (extraObj)
				argCopy.append(QVariant::fromValue(parent));
			Q_ASSERT(argCopy.size() <= 10);

			std::array<QGenericArgument, 10> gArgs;
			gArgs.fill(QGenericArgument{});
			for (auto pIdx = 0; pIdx < argCopy.size(); ++pIdx)
				gArgs[static_cast<size_t>(pIdx)] = {argCopy[pIdx].typeName(), argCopy[pIdx].constData()};
			auto object = metaObject->newInstance(gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4],
												  gArgs[5], gArgs[6], gArgs[7], gArgs[8], gArgs[9]);
			if (!object) {
				throw QJsonDeserializationException(QByteArray("Failed to construct object of type ") +
													metaObject->className() +
													QByteArray(" - deserialized constructor arguments are potentially invalid!"));
			}
			return object;
		}
	}

	throw QJsonDeserializationException{"Unable to find any constructor for " +
										className +
										" with " +
										QByteArray::number(arguments.size()) +
										" arguments and matching types!"};
}

QObject *QJsonObjectConverter::deserializeConstructedObject(const QCborValue &value, QObject *parent) const
{
	if (value.isNull())
		return nullptr;

	const auto cborArray = value.toArray();
	if (cborArray.size() != 2)
		throw QJsonDeserializationException{"The ConstructedObject tagged array must have exactly two elements!"};

	auto object = deserializeGenericObject(cborArray[0].toArray(), parent);
	if (!object)
		return nullptr;
	if (!cborArray[1].isNull())
		deserializeProperties(object->metaObject(), object, cborArray[1].toMap());
	return object;
}

void QJsonObjectConverter::deserializeProperties(const QMetaObject *metaObject, QObject *object, const QCborMap &value, bool isPoly) const
{
	auto validationFlags = helper()->getProperty("validationFlags").value<QJsonSerializerBase::ValidationFlags>();
	auto keepObjectName = helper()->getProperty("keepObjectName").toBool();

	// collect required properties, if set
	QSet<QByteArray> reqProps;
	if (validationFlags.testFlag(QJsonSerializerBase::ValidationFlag::AllProperties)) {
		const auto ignoreStoredAttribute = helper()->getProperty("ignoreStoredAttribute").toBool();
		auto i = QObject::staticMetaObject.indexOfProperty("objectName");
		if (!keepObjectName)
			i++;
		for (; i < metaObject->propertyCount(); i++) {
			auto property = metaObject->property(i);
			if(ignoreStoredAttribute || property.isStored())
				reqProps.insert(property.name());
		}
	}

	//now deserialize all json properties
	for (auto it = value.constBegin(); it != value.constEnd(); it++) {
		if (isPoly && it.key() == QStringLiteral("@class"))
			continue;

		const auto key = it.key().toString().toUtf8();
		const auto propIndex = metaObject->indexOfProperty(key);
		if (propIndex != -1) {
			const auto property = metaObject->property(propIndex);
			property.write(object, helper()->deserializeSubtype(property, it.value(), object));
			reqProps.remove(property.name());
		} else if (validationFlags.testFlag(QJsonSerializerBase::ValidationFlag::NoExtraProperties)) {
			throw QJsonDeserializationException("Found extra property " +
												key +
												" but extra properties are not allowed");
		} else
			object->setProperty(key, helper()->deserializeSubtype(QMetaType::UnknownType, it.value(), object, key));
	}

	//make shure all required properties have been read
	if (validationFlags.testFlag(QJsonSerializerBase::ValidationFlag::AllProperties) && !reqProps.isEmpty()) {
		throw QJsonDeserializationException(QByteArray("Not all properties for ") +
											metaObject->className() +
											QByteArray(" are present in the json object Missing properties: ") +
											reqProps.toList().join(", "));
	}
}
