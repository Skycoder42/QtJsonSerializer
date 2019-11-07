#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/bitarrayconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class BitarrayConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;

private:
	BitArrayConverter _converter;

	QBitArray generateAlternating(int len, int width = 2) const;
};

TypeConverter *BitarrayConverterTest::converter()
{
	return &_converter;
}

void BitarrayConverterTest::addConverterData()
{
	QTest::newRow("bitarray") << static_cast<int>(TypeConverter::Standard);
}

void BitarrayConverterTest::addMetaData()
{
	QTest::newRow("tagged") << static_cast<int>(QMetaType::QBitArray)
							<< static_cast<QCborTag>(CborSerializer::BitArray)
							<< QCborValue::ByteArray
							<< true
							<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("guessed") << static_cast<int>(QMetaType::UnknownType)
							 << static_cast<QCborTag>(CborSerializer::BitArray)
							 << QCborValue::ByteArray
							 << false
							 << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QByteArray)
								  << static_cast<QCborTag>(CborSerializer::BitArray)
								  << QCborValue::ByteArray
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << static_cast<int>(QMetaType::QBitArray)
								 << static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url)
								 << QCborValue::ByteArray
								 << true
								 << TypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("invalid.data") << static_cast<int>(QMetaType::QBitArray)
								  << static_cast<QCborTag>(CborSerializer::BitArray)
								  << QCborValue::String
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Negative;
}

void BitarrayConverterTest::addCommonSerData()
{
	QTest::newRow("basic.aligned") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QBitArray)
								   << QVariant{generateAlternating(24)}
								   << QCborValue{static_cast<QCborTag>(CborSerializer::BitArray), QByteArray("\x00UUU", 4)}
								   << QJsonValue{QStringLiteral("AFVVVQ")};
	QTest::newRow("basic.unaligned") << QVariantHash{}
									 << TestQ{}
									 << static_cast<QObject*>(nullptr)
									 << static_cast<int>(QMetaType::QBitArray)
									 << QVariant{generateAlternating(28, 4)}
									 << QCborValue{static_cast<QCborTag>(CborSerializer::BitArray), QByteArray("\x04\x11\x11\x11\x01", 5)}
									 << QJsonValue{QStringLiteral("BBEREQE")};
}

QBitArray BitarrayConverterTest::generateAlternating(int len, int width) const
{
	QBitArray array(len, false);
	for (auto i = 0; i < len; i += width)
		array.setBit(i, true);
	return array;
}

QTEST_MAIN(BitarrayConverterTest)

#include "tst_bitarrayconverter.moc"
