#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/bytearrayconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class BytearrayConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	BytearrayConverter _converter;
};

TypeConverter *BytearrayConverterTest::converter()
{
	return &_converter;
}

void BytearrayConverterTest::addConverterData()
{
	QTest::newRow("bytearray") << static_cast<int>(TypeConverter::Standard);
}

void BytearrayConverterTest::addMetaData()
{
	QTest::newRow("base64") << static_cast<int>(QMetaType::QByteArray)
							<< static_cast<QCborTag>(QCborKnownTags::ExpectedBase64)
							<< QCborValue::ByteArray
							<< true
							<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("base64url") << static_cast<int>(QMetaType::QByteArray)
							   << static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url)
							   << QCborValue::ByteArray
							   << true
							   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("base16") << static_cast<int>(QMetaType::QByteArray)
							<< static_cast<QCborTag>(QCborKnownTags::ExpectedBase16)
							<< QCborValue::ByteArray
							<< true
							<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("json") << static_cast<int>(QMetaType::QByteArray)
						  << static_cast<QCborTag>(CborSerializer::NoTag)
						  << QCborValue::String
						  << true
						  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("guess.base64") << static_cast<int>(QMetaType::UnknownType)
								  << static_cast<QCborTag>(QCborKnownTags::ExpectedBase64)
								  << QCborValue::ByteArray
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("guess.base64url") << static_cast<int>(QMetaType::UnknownType)
									 << static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url)
									 << QCborValue::ByteArray
									 << false
									 << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("guess.base16") << static_cast<int>(QMetaType::UnknownType)
								  << static_cast<QCborTag>(QCborKnownTags::ExpectedBase16)
								  << QCborValue::ByteArray
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QString)
								  << static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url)
								  << QCborValue::ByteArray
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.value") << static_cast<int>(QMetaType::QByteArray)
								   << static_cast<QCborTag>(QCborKnownTags::ExpectedBase64)
								   << QCborValue::String
								   << true
								   << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << static_cast<int>(QMetaType::QByteArray)
								 << static_cast<QCborTag>(QCborKnownTags::Base64)
								 << QCborValue::ByteArray
								 << true
								 << TypeConverter::DeserializationCapabilityResult::WrongTag    ;
}

void BytearrayConverterTest::addCommonSerData()
{
	QTest::newRow("basic") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << static_cast<int>(QMetaType::QByteArray)
						   << QVariant{QByteArrayLiteral("Hello World")}
						   << QCborValue{QByteArrayLiteral("Hello World")}
						   << QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ")};
	QTest::newRow("base64") << QVariantHash{
		{QStringLiteral("typeTag"), QVariant::fromValue(static_cast<QCborTag>(QCborKnownTags::ExpectedBase64))},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base64)}
	}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< static_cast<int>(QMetaType::QByteArray)
							<< QVariant{QByteArrayLiteral("Hello World")}
							<< QCborValue{}
							<< QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ=")};
	QTest::newRow("base64url") << QVariantHash{
		{QStringLiteral("typeTag"), QVariant::fromValue(static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url))},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base64url)}
	}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QByteArray)
							   << QVariant{QByteArrayLiteral("Hello World")}
							   << QCborValue{}
							   << QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ")};
	QTest::newRow("base16") << QVariantHash{
		{QStringLiteral("typeTag"), QVariant::fromValue(static_cast<QCborTag>(QCborKnownTags::ExpectedBase16))},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base16)}
	}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< static_cast<int>(QMetaType::QByteArray)
							<< QVariant{QByteArrayLiteral("Hello World")}
							<< QCborValue{}
							<< QJsonValue{QStringLiteral("48656c6c6f20576f726c64")};
}

void BytearrayConverterTest::addDeserData()
{
	QTest::newRow("validated.base64.invalid") << QVariantHash{
		{QStringLiteral("validateBase64"), false},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base64)}
	}
											  << TestQ{}
											  << static_cast<QObject*>(nullptr)
											  << static_cast<int>(QMetaType::QByteArray)
											  << QVariant{QByteArrayLiteral("Hello World8")}
											  << QCborValue{}
											  << QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ=42")};
	QTest::newRow("validated.base64.1") << QVariantHash{
		{QStringLiteral("validateBase64"), true},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base64)}
	}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QByteArray)
										<< QVariant{}
										<< QCborValue{}
										<< QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ=42")};
	QTest::newRow("validated.base64.2") << QVariantHash{
		{QStringLiteral("validateBase64"), true},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base64)}
	}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QByteArray)
										<< QVariant{}
										<< QCborValue{}
										<< QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ2=")};
	QTest::newRow("validated.base64.3") << QVariantHash{
		{QStringLiteral("validateBase64"), true},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base64)}
	}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QByteArray)
										<< QVariant{}
										<< QCborValue{}
										<< QJsonValue{QStringLiteral("SGVsbG%gV29ybGQ=")};
	QTest::newRow("validated.base64url.invalid") << QVariantHash{
		{QStringLiteral("validateBase64"), false},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base64url)}
	}
												 << TestQ{}
												 << static_cast<QObject*>(nullptr)
												 << static_cast<int>(QMetaType::QByteArray)
												 << QVariant{QByteArrayLiteral("Hello World")}
												 << QCborValue{}
												 << QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ")};
	QTest::newRow("validated.base64url") << QVariantHash{
		{QStringLiteral("validateBase64"), true},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base64url)}
	}
										 << TestQ{}
										 << static_cast<QObject*>(nullptr)
										 << static_cast<int>(QMetaType::QByteArray)
										 << QVariant{}
										 << QCborValue{}
										 << QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ")};
	QTest::newRow("validated.base16.1") << QVariantHash{
		{QStringLiteral("validateBase64"), true},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base16)}
	}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QByteArray)
										<< QVariant{}
										<< QCborValue{}
										<< QJsonValue{QStringLiteral("48656ggc6c6f2057,nmn6f726c64")};
	QTest::newRow("validated.base16.2") << QVariantHash{
		{QStringLiteral("validateBase64"), true},
		{QStringLiteral("byteArrayFormat"), QVariant::fromValue(JsonSerializer::ByteArrayFormat::Base16)}
	}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QByteArray)
										<< QVariant{}
										<< QCborValue{}
										<< QJsonValue{QStringLiteral("48656c6c6f20576f726c647")};
}

QTEST_MAIN(BytearrayConverterTest)

#include "tst_bytearrayconverter.moc"
