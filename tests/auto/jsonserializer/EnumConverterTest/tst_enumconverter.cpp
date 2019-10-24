#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonenumconverter_p.h>

#include "testclass.h"

class EnumConverterTest : public TypeConverterTestBase
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
	QJsonEnumConverter _converter;
};

void EnumConverterTest::initTest()
{
	auto ok = QMetaType::registerConverter<TestClass::TestFlags, int>([](TestClass::TestFlags flags) {
		return static_cast<int>(flags);
	});
	QVERIFY(ok);
	ok = QMetaType::registerConverter<int, TestClass::TestFlags>([](int flags) {
		return static_cast<TestClass::TestFlags>(flags);
	});
	QVERIFY(ok);
}

QJsonTypeConverter *EnumConverterTest::converter()
{
	return &_converter;
}

void EnumConverterTest::addConverterData()
{
	QTest::newRow("enum") << static_cast<int>(QJsonTypeConverter::Low);
}

void EnumConverterTest::addMetaData()
{
	QTest::newRow("enum.int.tagged") << qMetaTypeId<TestClass::TestEnum>()
									 << static_cast<QCborTag>(QCborSerializer::Enum)
									 << QCborValue::Integer
									 << true
									 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("enum.int.untagged") << qMetaTypeId<TestClass::TestEnum>()
									   << static_cast<QCborTag>(QCborSerializer::NoTag)
									   << QCborValue::Double
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("enum.int.invalid") << qMetaTypeId<TestClass::TestEnum>()
									  << static_cast<QCborTag>(QCborSerializer::Flags)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("enum.string.tagged") << qMetaTypeId<TestClass::TestEnum>()
										<< static_cast<QCborTag>(QCborSerializer::Enum)
										<< QCborValue::String
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("enum.string.untagged") << qMetaTypeId<TestClass::TestEnum>()
										  << static_cast<QCborTag>(QCborSerializer::NoTag)
										  << QCborValue::String
										  << true
										  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("enum.string.invalid") << static_cast<int>(QMetaType::UnknownType)
										 << static_cast<QCborTag>(QCborSerializer::Enum)
										 << QCborValue::String
										 << false
										 << QJsonTypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("flags.int.tagged") << qMetaTypeId<TestClass::TestFlags>()
									  << static_cast<QCborTag>(QCborSerializer::Flags)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("flags.int.untagged") << qMetaTypeId<TestClass::TestFlags>()
										<< static_cast<QCborTag>(QCborSerializer::NoTag)
										<< QCborValue::Double
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("flags.int.invalid") << qMetaTypeId<TestClass::TestFlags>()
									   << static_cast<QCborTag>(QCborSerializer::Enum)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("flags.string.tagged") << qMetaTypeId<TestClass::TestFlags>()
										 << static_cast<QCborTag>(QCborSerializer::Flags)
										 << QCborValue::String
										 << true
										 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("flags.string.untagged") << qMetaTypeId<TestClass::TestFlags>()
										   << static_cast<QCborTag>(QCborSerializer::NoTag)
										   << QCborValue::String
										   << true
										   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("flags.string.invalid") << static_cast<int>(QMetaType::UnknownType)
										  << static_cast<QCborTag>(QCborSerializer::Flags)
										  << QCborValue::String
										  << false
										  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;

}

void EnumConverterTest::addCommonSerData()
{
	QTest::newRow("enum.int") << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << qMetaTypeId<TestClass::TestEnum>()
							  << QVariant::fromValue(TestClass::TestEnum::Key1)
							  << QCborValue{static_cast<QCborTag>(QCborSerializer::Enum), static_cast<int>(TestClass::TestEnum::Key1)}
							  << QJsonValue{static_cast<int>(TestClass::TestEnum::Key1)};
	QTest::newRow("enum.string") << QVariantHash{{QStringLiteral("enumAsString"), true}}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << qMetaTypeId<TestClass::TestEnum>()
								 << QVariant::fromValue(TestClass::TestEnum::Key4)
								 << QCborValue{static_cast<QCborTag>(QCborSerializer::Enum), QStringLiteral("Key4")}
								 << QJsonValue{QStringLiteral("Key4")};

	QTest::newRow("flags.int") << QVariantHash{}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<TestClass::TestFlags>()
							   << QVariant::fromValue(TestClass::TestFlag::Flag2 | TestClass::TestFlag::Flag4)
							   << QCborValue{static_cast<QCborTag>(QCborSerializer::Flags), static_cast<int>(TestClass::TestFlag::Flag2 | TestClass::TestFlag::Flag4)}
							   << QJsonValue{static_cast<int>(TestClass::TestFlag::Flag2 | TestClass::TestFlag::Flag4)};
	QTest::newRow("flags.string.simple") << QVariantHash{{QStringLiteral("enumAsString"), true}}
										 << TestQ{}
										 << static_cast<QObject*>(nullptr)
										 << qMetaTypeId<TestClass::TestFlags>()
										 << QVariant::fromValue<TestClass::TestFlags>(TestClass::TestFlag::Flag1)
										 << QCborValue{static_cast<QCborTag>(QCborSerializer::Flags), QStringLiteral("Flag1")}
										 << QJsonValue{QStringLiteral("Flag1")};
	QTest::newRow("flags.string.complex") << QVariantHash{{QStringLiteral("enumAsString"), true}}
										  << TestQ{}
										  << static_cast<QObject*>(nullptr)
										  << qMetaTypeId<TestClass::TestFlags>()
										  << QVariant::fromValue(TestClass::TestFlag::Flag2 | TestClass::TestFlag::Flag4)
										  << QCborValue{static_cast<QCborTag>(QCborSerializer::Flags), QStringLiteral("Flag2|Flag4")}
										  << QJsonValue{QStringLiteral("Flag2|Flag4")};
}

void EnumConverterTest::addDeserData()
{

	QTest::newRow("enum.int.invalid") << QVariantHash{}
									  << TestQ{}
									  << static_cast<QObject*>(nullptr)
									  << qMetaTypeId<TestClass::TestEnum>()
									  << QVariant{}
									  << QCborValue{20}
									  << QJsonValue{20};
	QTest::newRow("enum.string.invalid") << QVariantHash{{QStringLiteral("enumAsString"), true}}
										 << TestQ{}
										 << static_cast<QObject*>(nullptr)
										 << qMetaTypeId<TestClass::TestEnum>()
										 << QVariant{}
										 << QCborValue{QStringLiteral("invalid")}
										 << QJsonValue{QStringLiteral("invalid")};
	QTest::newRow("flags.double.invalid") << QVariantHash{}
										  << TestQ{}
										  << static_cast<QObject*>(nullptr)
										  << qMetaTypeId<TestClass::TestFlags>()
										  << QVariant{}
										  << QCborValue{}
										  << QJsonValue{4.2};
	QTest::newRow("flags.string.invalid") << QVariantHash{{QStringLiteral("enumAsString"), true}}
										  << TestQ{}
										  << static_cast<QObject*>(nullptr)
										  << qMetaTypeId<TestClass::TestFlags>()
										  << QVariant{}
										  << QCborValue{QStringLiteral("invalid")}
										  << QJsonValue{QStringLiteral("invalid")};
}

QTEST_MAIN(EnumConverterTest)

#include "tst_enumconverter.moc"
