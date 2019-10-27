#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonversionnumberconverter_p.h>

class VersionConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	QJsonVersionNumberConverter _converter;
};

void VersionConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<QVersionNumber>();
}

QJsonTypeConverter *VersionConverterTest::converter()
{
	return &_converter;
}

void VersionConverterTest::addConverterData()
{
	QTest::newRow("version") << static_cast<int>(QJsonTypeConverter::Standard);
}

void VersionConverterTest::addMetaData()
{
	QTest::newRow("array.tagged") << qMetaTypeId<QVersionNumber>()
								  << static_cast<QCborTag>(QCborSerializer::VersionNumber)
								  << QCborValue::Array
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("array.untagged") << qMetaTypeId<QVersionNumber>()
									<< static_cast<QCborTag>(QCborSerializer::NoTag)
									<< QCborValue::Array
									<< true
									<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("array.guessed") << static_cast<int>(QMetaType::UnknownType)
								   << static_cast<QCborTag>(QCborSerializer::VersionNumber)
								   << QCborValue::Array
								   << false
								   << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("string.tagged") << qMetaTypeId<QVersionNumber>()
								   << static_cast<QCborTag>(QCborSerializer::VersionNumber)
								   << QCborValue::String
								   << true
								   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("string.untagged") << qMetaTypeId<QVersionNumber>()
									 << static_cast<QCborTag>(QCborSerializer::NoTag)
									 << QCborValue::String
									 << true
									 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("string.guessed") << static_cast<int>(QMetaType::UnknownType)
									<< static_cast<QCborTag>(QCborSerializer::VersionNumber)
									<< QCborValue::String
									<< false
									<< QJsonTypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QString)
								  << static_cast<QCborTag>(QCborSerializer::VersionNumber)
								  << QCborValue::String
								  << false
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.data") << qMetaTypeId<QVersionNumber>()
								  << static_cast<QCborTag>(QCborSerializer::VersionNumber)
								  << QCborValue::ByteArray
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << qMetaTypeId<QVersionNumber>()
								  << static_cast<QCborTag>(QCborSerializer::Homogeneous)
								  << QCborValue::Array
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
}

void VersionConverterTest::addCommonSerData()
{
	QTest::newRow("array") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<QVersionNumber>()
						   << QVariant::fromValue(QVersionNumber{1, 2, 3, 4, 5})
						   << QCborValue{static_cast<QCborTag>(QCborSerializer::VersionNumber), QCborArray{1, 2, 3, 4, 5}}
						   << QJsonValue{QJsonArray{1, 2, 3, 4, 5}};
	QTest::newRow("array.empty") << QVariantHash{}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << qMetaTypeId<QVersionNumber>()
								 << QVariant::fromValue(QVersionNumber{})
								 << QCborValue{static_cast<QCborTag>(QCborSerializer::VersionNumber), QCborArray{}}
								 << QJsonValue{QJsonArray{}};
	QTest::newRow("string") << QVariantHash{{QStringLiteral("versionAsString"), true}}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< qMetaTypeId<QVersionNumber>()
							<< QVariant::fromValue(QVersionNumber{1, 2, 3, 4, 5})
							<< QCborValue{static_cast<QCborTag>(QCborSerializer::VersionNumber), QStringLiteral("1.2.3.4.5")}
							<< QJsonValue{QStringLiteral("1.2.3.4.5")};
	QTest::newRow("string.empty") << QVariantHash{{QStringLiteral("versionAsString"), true}}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << qMetaTypeId<QVersionNumber>()
								  << QVariant::fromValue(QVersionNumber{})
								  << QCborValue{static_cast<QCborTag>(QCborSerializer::VersionNumber), QString{}}
								  << QJsonValue{QString{}};
}

void VersionConverterTest::addDeserData()
{
	QTest::newRow("array.invalid") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<QVersionNumber>()
								   << QVariant{}
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::VersionNumber), QCborArray{1, 2, 3, 4.5}}
								   << QJsonValue{QJsonArray{1, 2, 3, 4.5}};
	QTest::newRow("string.suffixed") << QVariantHash{{QStringLiteral("versionAsString"), true}}
									 << TestQ{}
									 << static_cast<QObject*>(nullptr)
									 << qMetaTypeId<QVersionNumber>()
									 << QVariant::fromValue(QVersionNumber{1, 2, 3})
									 << QCborValue{static_cast<QCborTag>(QCborSerializer::VersionNumber), QStringLiteral("1.2.3-r1")}
									 << QJsonValue{QStringLiteral("1.2.3-r1")};
	QTest::newRow("string.invalid") << QVariantHash{{QStringLiteral("versionAsString"), true}}
									<< TestQ{}
									<< static_cast<QObject*>(nullptr)
									<< qMetaTypeId<QVersionNumber>()
									<< QVariant{}
									<< QCborValue{static_cast<QCborTag>(QCborSerializer::VersionNumber), QStringLiteral("A1.4.5")}
									<< QJsonValue{QStringLiteral("A1.4.5")};
}

QTEST_MAIN(VersionConverterTest)

#include "tst_versionconverter.moc"
