#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/versionnumberconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class VersionConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	VersionNumberConverter _converter;
};

void VersionConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<QVersionNumber>();
}

TypeConverter *VersionConverterTest::converter()
{
	return &_converter;
}

void VersionConverterTest::addConverterData()
{
	QTest::newRow("version") << static_cast<int>(TypeConverter::Standard);
}

void VersionConverterTest::addMetaData()
{
	QTest::newRow("array.tagged") << qMetaTypeId<QVersionNumber>()
								  << static_cast<QCborTag>(CborSerializer::VersionNumber)
								  << QCborValue::Array
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("array.untagged") << qMetaTypeId<QVersionNumber>()
									<< static_cast<QCborTag>(CborSerializer::NoTag)
									<< QCborValue::Array
									<< true
									<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("array.guessed") << static_cast<int>(QMetaType::UnknownType)
								   << static_cast<QCborTag>(CborSerializer::VersionNumber)
								   << QCborValue::Array
								   << false
								   << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("string.tagged") << qMetaTypeId<QVersionNumber>()
								   << static_cast<QCborTag>(CborSerializer::VersionNumber)
								   << QCborValue::String
								   << true
								   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("string.untagged") << qMetaTypeId<QVersionNumber>()
									 << static_cast<QCborTag>(CborSerializer::NoTag)
									 << QCborValue::String
									 << true
									 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("string.guessed") << static_cast<int>(QMetaType::UnknownType)
									<< static_cast<QCborTag>(CborSerializer::VersionNumber)
									<< QCborValue::String
									<< false
									<< TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QString)
								  << static_cast<QCborTag>(CborSerializer::VersionNumber)
								  << QCborValue::String
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.data") << qMetaTypeId<QVersionNumber>()
								  << static_cast<QCborTag>(CborSerializer::VersionNumber)
								  << QCborValue::ByteArray
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << qMetaTypeId<QVersionNumber>()
								  << static_cast<QCborTag>(CborSerializer::Homogeneous)
								  << QCborValue::Array
								  << true
								  << TypeConverter::DeserializationCapabilityResult::WrongTag;
}

void VersionConverterTest::addCommonSerData()
{
	QTest::newRow("array") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<QVersionNumber>()
						   << QVariant::fromValue(QVersionNumber{1, 2, 3, 4, 5})
						   << QCborValue{static_cast<QCborTag>(CborSerializer::VersionNumber), QCborArray{1, 2, 3, 4, 5}}
						   << QJsonValue{QJsonArray{1, 2, 3, 4, 5}};
	QTest::newRow("array.empty") << QVariantHash{}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << qMetaTypeId<QVersionNumber>()
								 << QVariant::fromValue(QVersionNumber{})
								 << QCborValue{static_cast<QCborTag>(CborSerializer::VersionNumber), QCborArray{}}
								 << QJsonValue{QJsonArray{}};
	QTest::newRow("string") << QVariantHash{{QStringLiteral("versionAsString"), true}}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< qMetaTypeId<QVersionNumber>()
							<< QVariant::fromValue(QVersionNumber{1, 2, 3, 4, 5})
							<< QCborValue{static_cast<QCborTag>(CborSerializer::VersionNumber), QStringLiteral("1.2.3.4.5")}
							<< QJsonValue{QStringLiteral("1.2.3.4.5")};
	QTest::newRow("string.empty") << QVariantHash{{QStringLiteral("versionAsString"), true}}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << qMetaTypeId<QVersionNumber>()
								  << QVariant::fromValue(QVersionNumber{})
								  << QCborValue{static_cast<QCborTag>(CborSerializer::VersionNumber), QString{}}
								  << QJsonValue{QString{}};
}

void VersionConverterTest::addDeserData()
{
	QTest::newRow("array.invalid") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<QVersionNumber>()
								   << QVariant{}
								   << QCborValue{static_cast<QCborTag>(CborSerializer::VersionNumber), QCborArray{1, 2, 3, 4.5}}
								   << QJsonValue{QJsonArray{1, 2, 3, 4.5}};
	QTest::newRow("string.suffixed") << QVariantHash{{QStringLiteral("versionAsString"), true}}
									 << TestQ{}
									 << static_cast<QObject*>(nullptr)
									 << qMetaTypeId<QVersionNumber>()
									 << QVariant::fromValue(QVersionNumber{1, 2, 3})
									 << QCborValue{static_cast<QCborTag>(CborSerializer::VersionNumber), QStringLiteral("1.2.3-r1")}
									 << QJsonValue{QStringLiteral("1.2.3-r1")};
	QTest::newRow("string.invalid") << QVariantHash{{QStringLiteral("versionAsString"), true}}
									<< TestQ{}
									<< static_cast<QObject*>(nullptr)
									<< qMetaTypeId<QVersionNumber>()
									<< QVariant{}
									<< QCborValue{static_cast<QCborTag>(CborSerializer::VersionNumber), QStringLiteral("A1.4.5")}
									<< QJsonValue{QStringLiteral("A1.4.5")};
}

QTEST_MAIN(VersionConverterTest)

#include "tst_versionconverter.moc"
