#include "bitarrayconverter_p.h"
#include "cborserializer.h"
#include <QtCore/QBitArray>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

bool BitArrayConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QBitArray;
}

QList<QCborTag> BitArrayConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {static_cast<QCborTag>(CborSerializer::BitArray)};
}

QList<QCborValue::Type> BitArrayConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::ByteArray};
}

int BitArrayConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	if (tag == static_cast<QCborTag>(CborSerializer::BitArray) &&
		dataType == QCborValue::ByteArray)
		return QMetaType::QBitArray;
	else
		return QMetaType::UnknownType;
}

QCborValue BitArrayConverter::serialize(int propertyType, const QVariant &value) const
{
	Q_UNUSED(propertyType)
	const auto bitArray = value.value<QBitArray>();
	if (bitArray.isEmpty())
		return {static_cast<QCborTag>(CborSerializer::BitArray), QByteArray{}};
	else {
		const auto byteLen = bitArray.size() % 8 == 0 ?
													  bitArray.size() / 8 :
													  (bitArray.size() / 8) + 1;
		QByteArray cData(byteLen + 1, 0);
		cData[0] = static_cast<char>(bitArray.size() % 8);
		memcpy(cData.data() + 1, bitArray.bits(), static_cast<size_t>(byteLen));
		return {static_cast<QCborTag>(CborSerializer::BitArray), cData};
	}
}

QVariant BitArrayConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	Q_UNUSED(propertyType)
	Q_UNUSED(parent)

	const auto cData = (value.isTag() ? value.taggedValue() : value).toByteArray();
	if (cData.isEmpty())
		return QBitArray{};
	else {
		const auto byteLen = cData.size() - 1;
		const auto sSize = static_cast<int>(cData[0]);
		if (sSize == 0)
			return QBitArray::fromBits(cData.data() + 1, byteLen * 8);
		else
			return QBitArray::fromBits(cData.data() + 1, (byteLen - 1) * 8 + sSize);
	}
}

QVariant BitArrayConverter::deserializeJson(int propertyType, const QCborValue &value, QObject *parent) const
{
	return deserializeCbor(propertyType,
						   QByteArray::fromBase64(value.toString().toUtf8(), QByteArray::Base64UrlEncoding),
						   parent);
}
