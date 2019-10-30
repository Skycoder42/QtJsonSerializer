#include "qjsonmultimapconverter_p.h"
#include "qjsonserializerexception.h"
#include "qcborserializer.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

bool QJsonMultiMapConverter::canConvert(int metaTypeId) const
{
	const QVariant tValue{metaTypeId, nullptr};
	return (tValue.canConvert(QMetaType::QVariantMap) ||
			tValue.canConvert(QMetaType::QVariantHash)) &&
		   QAssociativeWriter::canWrite(metaTypeId);
}

QList<QCborTag> QJsonMultiMapConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {
		NoTag,
		static_cast<QCborTag>(QCborSerializer::MultiMap),
		static_cast<QCborTag>(QCborSerializer::ExplicitMap)
	};
}

QList<QCborValue::Type> QJsonMultiMapConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Map, QCborValue::Array};
}

QCborValue QJsonMultiMapConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto info = QAssociativeWriter::getInfo(propertyType);

	// verify is readable
	if (!value.canConvert(QMetaType::QVariantMap) &&
		!value.canConvert(QMetaType::QVariantHash)) {
		throw QJsonSerializationException(QByteArray("Given type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" cannot be processed via QAssociativeIterable - make shure to register the container type via Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE"));
	}

	// write from map to cbor
	const auto mapMode = helper()->getProperty("multiMapMode").value<QJsonSerializerBase::MultiMapMode>();
	const auto iterable = value.value<QAssociativeIterable>();
	switch (mapMode) {
	case QJsonSerializerBase::MultiMapMode::Map:
	case QJsonSerializerBase::MultiMapMode::DenseMap: {
		QCborMap cborMap;
		for (auto it = iterable.begin(), end = iterable.end(); it != end; ++it) {
			const QByteArray keyStr = "[" + it.key().toString().toUtf8() + "]";
			const auto key = helper()->serializeSubtype(info.keyType, it.key(), keyStr + ".key");
			auto mValueRef = cborMap[key];
			const auto vType = mapMode == QJsonSerializerBase::MultiMapMode::DenseMap ?
				mValueRef.type() :
				QCborValue::Array;
			switch (vType) {
			case QCborValue::Array: {
				auto mArray = mValueRef.toArray();
				mValueRef = QCborValue{}; // "clear" the array spot, reducing the ref cnt on mArray to 1, so stuff is added without copying
				mArray.append(helper()->serializeSubtype(info.valueType, it.value(), keyStr + ".value"));
				mValueRef = mArray;
				break;
			}
			case QCborValue::Undefined:
				mValueRef = helper()->serializeSubtype(info.valueType, it.value(), keyStr + ".value");
				break;
			default: {
				QCborArray mArray {mValueRef};
				mArray.append(helper()->serializeSubtype(info.valueType, it.value(), keyStr + ".value"));
				mValueRef = mArray;
				break;
			}
			}
		}
		return {static_cast<QCborTag>(QCborSerializer::MultiMap), cborMap};
	}
	case QJsonSerializerBase::MultiMapMode::List: {
		QCborArray cborArray;
		for (auto it = iterable.begin(), end = iterable.end(); it != end; ++it) {
			const QByteArray keyStr = "[" + it.key().toString().toUtf8() + "]";
			cborArray.append(QCborArray{
				helper()->serializeSubtype(info.keyType, it.key(), keyStr + ".key"),
				helper()->serializeSubtype(info.valueType, it.value(), keyStr + ".value")
			});
		}
		return {static_cast<QCborTag>(QCborSerializer::MultiMap), cborArray};
	}
	default:
		Q_UNREACHABLE();
		return {};
	}
}

QVariant QJsonMultiMapConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	// generate the map
	QVariant map{propertyType, nullptr};
	auto writer = QAssociativeWriter::getWriter(map);
	if (!writer) {
		throw QJsonDeserializationException(QByteArray("Given type ") +
											QMetaType::typeName(propertyType) +
											QByteArray(" cannot be accessed via QAssociativeWriter - make shure to register it via QJsonSerializerBase::registerMapConverters"));
	}

	// write from cbor into the map
	const auto info = writer->info();
	const auto cValue = (value.isTag() ? value.taggedValue() : value);
	switch (cValue.type()) {
	case QCborValue::Map: {
		for (const auto entry : cValue.toMap()) {
			const QByteArray keyStr = "[" + entry.first.toVariant().toString().toUtf8() + "]";
			const auto key = helper()->deserializeSubtype(info.keyType, entry.first, parent, keyStr + ".key");
			if (entry.second.isArray()) {
				auto cnt = 0;
				for (const auto aValue : entry.second.toArray())
					writer->add(key, helper()->deserializeSubtype(info.valueType, aValue, parent, keyStr + ".value[" + QByteArray::number(cnt++) + "]"));
			} else
				writer->add(key, helper()->deserializeSubtype(info.valueType, entry.second, parent, keyStr + ".value"));
		}
		break;
	}
	case QCborValue::Array: {
		for (const auto aValue : cValue.toArray()) {
			const auto vPair = aValue.toArray();
			if (vPair.size() != 2)
				throw QJsonDeserializationException("CBOR/JSON array must have exactly 2 elements to be read as a value of a multi map");
			const QByteArray keyStr = "[" + vPair[0].toVariant().toString().toUtf8() + "]";
			writer->add(helper()->deserializeSubtype(info.keyType, vPair[0], parent, keyStr + ".key"),
						helper()->deserializeSubtype(info.valueType, vPair[1], parent, keyStr + ".value"));
		}
		break;
	}
	default:
		throw QJsonDeserializationException("Unsupported CBOR/JSON-Type: " + QByteArray::number(value.type()));
	}

	return map;
}
