#include "gadgetconverter_p.h"
#include "exception.h"
#include "serializerbase_p.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QSet>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool GadgetConverter::canConvert(int metaTypeId) const
{
	// exclude a few Qt gadgets that have no properties and thus need to be handled otherwise
	static const QSet<int> gadgetExceptions {
		QMetaType::QKeySequence,
		QMetaType::QFont,
		QMetaType::QLocale,
	};
	if(gadgetExceptions.contains(metaTypeId))
		return false;


	const auto flags = QMetaType(metaTypeId).flags();
	return flags.testFlag(QMetaType::IsGadget) ||
			flags.testFlag(QMetaType::PointerToGadget);
}

QList<QCborValue::Type> GadgetConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(tag)
	if (QMetaType(metaTypeId).flags().testFlag(QMetaType::PointerToGadget))
		return {QCborValue::Map, QCborValue::Null};
	else
		return {QCborValue::Map};
}

QCborValue GadgetConverter::serialize(int propertyType, const QVariant &value) const
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	const auto metaObject = QMetaType::metaObjectForType(propertyType);
#else
	auto metaType = QMetaType(propertyType);
	const auto metaObject = metaType.metaObject();
#endif

	if (!metaObject)
		throw SerializationException(QByteArray("Unable to get metaobject for type ") + QMetaTypeName(propertyType));
	const auto isPtr = QMetaType(propertyType).flags().testFlag(QMetaType::PointerToGadget);

	auto gValue = value;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	if (!gValue.convert(propertyType))
#else
	if (!gValue.convert(metaType))
#endif
		throw SerializationException(QByteArray("Data is not of the required gadget type ") + QMetaTypeName(propertyType));
	const void *gadget = nullptr;
	if (isPtr) {
		// with pointers, null gadgets are allowed
		gadget = *reinterpret_cast<const void* const *>(gValue.constData());
		if (!gadget)
			return QCborValue::Null;
	} else
		gadget = gValue.constData();
	if (!gadget)
		throw SerializationException(QByteArray("Unable to get address of gadget ") + QMetaTypeName(propertyType));

	QCborMap cborMap;
	//go through all properties and try to serialize them
	const auto ignoreStoredAttribute = helper()->getProperty("ignoreStoredAttribute").toBool();
	for (auto i = 0; i < metaObject->propertyCount(); i++) {
		auto property = metaObject->property(i);
		if (ignoreStoredAttribute || property.isStored())
			cborMap[QString::fromUtf8(property.name())] = helper()->serializeSubtype(property, property.readOnGadget(gadget));
	}

	return cborMap;
}

QVariant GadgetConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(parent)  // gadgets neither have nor serve as parent
	const auto isPtr = QMetaType(propertyType).flags().testFlag(QMetaType::PointerToGadget);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        const auto metaObject = QMetaType::metaObjectForType(propertyType);
#else
        auto metaType = QMetaType(propertyType);
        const auto metaObject = metaType.metaObject();
#endif
	
	if (!metaObject)
		throw DeserializationException(QByteArray("Unable to get metaobject for gadget type") + QMetaTypeName(propertyType));

	auto cValue = value.isTag() ? value.taggedValue() : value;
	QVariant gadget;
	void *gadgetPtr = nullptr;
	if (isPtr) {
		if (cValue.isNull())
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			return QVariant{propertyType, nullptr};  // initialize an empty (nullptr) variant
		const auto gadgetType = QMetaType::type(metaObject->className());
		if (gadgetType == QMetaType::UnknownType)
			throw DeserializationException(QByteArray("Unable to get type of gadget from gadget-pointer type") + QMetaTypeName(propertyType));
		gadgetPtr = QMetaType::create(gadgetType);
		gadget = QVariant{propertyType, &gadgetPtr};
#else
			return QVariant{metaType, nullptr};  // initialize an empty (nullptr) variant
		auto gadgetMetaType = QMetaType::fromName(metaObject->className());
		if (!gadgetMetaType.isValid())
			throw DeserializationException(QByteArray("Unable to get type of gadget from gadget-pointer type") + QMetaTypeName(propertyType));
		gadgetPtr = gadgetMetaType.create();
		gadget = QVariant{metaType, &gadgetPtr};
#endif
	} else {
		if (cValue.isNull())
			return QVariant{};  // return to allow default null for gadgets. If not allowed, this will fail, as a null variant cannot be converted to a gadget
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		gadget = QVariant{propertyType, nullptr};
#else
		gadget = QVariant{metaType, nullptr};
#endif
		gadgetPtr = gadget.data();
	}

	if (!gadgetPtr) {
		throw DeserializationException(QByteArray("Failed to construct gadget of type ") +
											QMetaTypeName(propertyType) +
											QByteArray(". Does it have a default constructor?"));
	}

	const auto validationFlags = helper()->getProperty("validationFlags").value<SerializerBase::ValidationFlags>();
	const auto ignoreStoredAttribute = helper()->getProperty("ignoreStoredAttribute").toBool();

	// collect required properties, if set
	QSet<QByteArray> reqProps;
	if (validationFlags.testFlag(SerializerBase::ValidationFlag::AllProperties)) {
		for (auto i = 0; i < metaObject->propertyCount(); i++) {
			auto property = metaObject->property(i);
			if (ignoreStoredAttribute || property.isStored())
				reqProps.insert(property.name());
		}
	}

	// now deserialize all json properties
	const auto cborMap = cValue.toMap();
	for (auto it = cborMap.constBegin(); it != cborMap.constEnd(); it++) {
		const auto key = it.key().toString().toUtf8();
		const auto propIndex = metaObject->indexOfProperty(key);
		if (propIndex != -1) {
			const auto property = metaObject->property(propIndex);
			property.writeOnGadget(gadgetPtr, helper()->deserializeSubtype(property, it.value(), nullptr));
			reqProps.remove(property.name());
		} else if (validationFlags.testFlag(SerializerBase::ValidationFlag::NoExtraProperties)) {
			throw DeserializationException("Found extra property " +
												key +
												" but extra properties are not allowed");
		}
	}

	// make sure all required properties have been read
	if (validationFlags.testFlag(SerializerBase::ValidationFlag::AllProperties) && !reqProps.isEmpty()) {
		throw DeserializationException(QByteArray("Not all properties for ") +
											metaObject->className() +
											QByteArray(" are present in the json object. Missing properties: ") +
											reqProps.values().join(", "));
	}

	return gadget;
}
