#include "objectconverter_p.h"
#include "exception.h"
#include "cborserializer.h"

#include <array>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

Q_LOGGING_CATEGORY(QtJsonSerializer::TypeConverters::logObjConverter, "qt.jsonserializer.converter.object")

bool ObjectConverter::canConvert(int metaTypeId) const
{
	auto flags = QMetaType::typeFlags(metaTypeId);
	return flags.testFlag(QMetaType::PointerToQObject);
}

QList<QCborTag> ObjectConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {
		NoTag,
		static_cast<QCborTag>(CborSerializer::GenericObject),
		static_cast<QCborTag>(CborSerializer::ConstructedObject)
	};
}

QList<QCborValue::Type> ObjectConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	switch (static_cast<quint64>(tag)) {
	case CborSerializer::GenericObject:
		return {QCborValue::Array};
	case CborSerializer::ConstructedObject:
		return {QCborValue::Array, QCborValue::Null};
	default:
		return {QCborValue::Map, QCborValue::Null};
	}
}

int ObjectConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	Q_UNUSED(dataType)
	switch (static_cast<quint64>(tag)) {
	case CborSerializer::GenericObject:
	case CborSerializer::ConstructedObject:
		return QMetaType::QObjectStar;
	default:
		return QMetaType::UnknownType;
	}
}

QCborValue ObjectConverter::serialize(int propertyType, const QVariant &value) const
{
	auto object = value.value<QObject*>();
	if (!object)
		return QCborValue::Null;
	QCborMap cborMap;

	// get the metaobject, based on polymorphism
	const QMetaObject *metaObject = nullptr;
	auto poly = static_cast<SerializerBase::Polymorphing>(helper()->getProperty("polymorphing").toInt());
	auto isPoly = false;
	switch (poly) {
	case SerializerBase::Polymorphing::Disabled:
		isPoly = false;
		break;
	case SerializerBase::Polymorphing::Enabled:
		isPoly = polyMetaObject(object);
		break;
	case SerializerBase::Polymorphing::Forced:
		isPoly = true;
		break;
	default:
		Q_UNREACHABLE();
		break;
	}

	if (isPoly) {
		metaObject = object->metaObject();
		//first: pass the class name
		cborMap[QStringLiteral("@class")] = QString::fromUtf8(metaObject->className());
	} else
		metaObject = QMetaType::metaObjectForType(propertyType);
	if (!metaObject)
		throw SerializationException(QByteArray("Unable to get metaobject for type ") + QMetaType::typeName(propertyType));

	//go through all properties and try to serialize them
	const auto keepObjectName = helper()->getProperty("keepObjectName").toBool();
	const auto ignoreStoredAttribute = helper()->getProperty("ignoreStoredAttribute").toBool();
	auto i = QObject::staticMetaObject.indexOfProperty("objectName");
	if (!keepObjectName)
		i++;
	for(; i < metaObject->propertyCount(); i++) {
		auto property = metaObject->property(i);
		if (ignoreStoredAttribute || property.isStored())
			cborMap[QString::fromUtf8(property.name())] = helper()->serializeSubtype(property, property.read(object));
	}

	return cborMap;
}

QVariant ObjectConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	if ((value.isTag() ? value.taggedValue() : value).isNull())
		return QVariant::fromValue<QObject*>(nullptr);

	QCborMap cborMap;
	if (value.isTag()) {
		if (value.tag() == static_cast<QCborTag>(CborSerializer::GenericObject))
			return QVariant::fromValue(deserializeGenericObject(value.taggedValue().toArray(), parent));
		else if (value.tag() == static_cast<QCborTag>(CborSerializer::ConstructedObject))
			return QVariant::fromValue(deserializeConstructedObject(value.taggedValue(), parent));
		else
			cborMap = value.taggedValue().toMap();
	} else
		cborMap = value.toMap();

	auto poly = static_cast<SerializerBase::Polymorphing>(helper()->getProperty("polymorphing").toInt());

	auto metaObject = QMetaType::metaObjectForType(propertyType);
	if (!metaObject)
		throw DeserializationException(QByteArray("Unable to get metaobject for type ") + QMetaType::typeName(propertyType));

	// try to get the polymorphic metatype (if allowed)
	auto isPoly = false;
	if (poly != SerializerBase::Polymorphing::Disabled) {
		if (cborMap.contains(QStringLiteral("@class"))) {
			isPoly = true;
			QByteArray classField = cborMap[QStringLiteral("@class")].toString().toUtf8() + "*";  // add the star
			auto typeId = QMetaType::type(classField.constData());
			auto nMeta = QMetaType::metaObjectForType(typeId);
			if (!nMeta)
				throw DeserializationException("Unable to find class requested from json \"@class\" property: " + classField);
			if (!nMeta->inherits(metaObject)) {
				throw DeserializationException("Requested class from \"@class\" field, " +
													classField +
													QByteArray(", does not inhert the property type ") +
													QMetaType::typeName(propertyType));
			}
			metaObject = nMeta;
		} else if (poly == SerializerBase::Polymorphing::Forced)
			throw DeserializationException("Json does not contain the \"@class\" field, but forced polymorphism requires it");
	}

	// try to construct the object
	auto object = metaObject->newInstance(Q_ARG(QObject*, parent));
	if (!object) {
		throw DeserializationException(QByteArray("Failed to construct object of type ") +
											metaObject->className() +
											QByteArray(" (Does the constructor \"Q_INVOKABLE class(QObject*);\" exist?)"));
	}

	deserializeProperties(metaObject, object, cborMap, isPoly);
	return QVariant::fromValue(object);
}

bool ObjectConverter::polyMetaObject(QObject *object) const
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
			qCWarning(logObjConverter) << "Invalid value for polymorphic classinfo on object type" << meta->className() << "ignored";
	}

	//default: the class
	return false;// use the class
}

QObject *ObjectConverter::deserializeGenericObject(const QCborArray &value, QObject *parent) const
{
	if (value.size() == 0)
		throw DeserializationException{"A GenericObject requires at least one argument, specifying the object name"};

	// find a meta object
	QByteArray className = value.first().toString().toUtf8() + "*";  // add the star
	auto typeId = QMetaType::type(className.constData());
	auto metaObject = QMetaType::metaObjectForType(typeId);
	if (!metaObject)
		throw DeserializationException("Unable to find class requested from GenericObject tagged array: " + className);

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
				throw DeserializationException(QByteArray("Failed to construct object of type ") +
													metaObject->className() +
													QByteArray(" - deserialized constructor arguments are potentially invalid!"));
			}
			return object;
		}
	}

	throw DeserializationException{"Unable to find any constructor for " +
										className +
										" with " +
										QByteArray::number(arguments.size()) +
										" arguments and matching types!"};
}

QObject *ObjectConverter::deserializeConstructedObject(const QCborValue &value, QObject *parent) const
{
	if (value.isNull())
		return nullptr;

	const auto cborArray = value.toArray();
	if (cborArray.size() != 2)
		throw DeserializationException{"The ConstructedObject tagged array must have exactly two elements!"};

	auto object = deserializeGenericObject(cborArray[0].toArray(), parent);
	if (!object)
		return nullptr;
	if (!cborArray[1].isNull())
		deserializeProperties(object->metaObject(), object, cborArray[1].toMap());
	return object;
}

void ObjectConverter::deserializeProperties(const QMetaObject *metaObject, QObject *object, const QCborMap &value, bool isPoly) const
{
	auto validationFlags = helper()->getProperty("validationFlags").value<SerializerBase::ValidationFlags>();
	auto keepObjectName = helper()->getProperty("keepObjectName").toBool();

	// collect required properties, if set
	QSet<QByteArray> reqProps;
	if (validationFlags.testFlag(SerializerBase::ValidationFlag::AllProperties)) {
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
		} else if (validationFlags.testFlag(SerializerBase::ValidationFlag::NoExtraProperties)) {
			throw DeserializationException("Found extra property " +
												key +
												" but extra properties are not allowed");
		} else
			object->setProperty(key, helper()->deserializeSubtype(QMetaType::UnknownType, it.value(), object, key));
	}

	//make shure all required properties have been read
	if (validationFlags.testFlag(SerializerBase::ValidationFlag::AllProperties) && !reqProps.isEmpty()) {
		throw DeserializationException(QByteArray("Not all properties for ") +
											metaObject->className() +
											QByteArray(" are present in the json object Missing properties: ") +
											reqProps.toList().join(", "));
	}
}
