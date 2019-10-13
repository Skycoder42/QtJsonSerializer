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

private:
	QJsonEnumConverter _converter;
};

void EnumConverterTest::initTest()
{
	QMetaType::registerConverter<TestClass::TestFlags, int>([](TestClass::TestFlags flags) {
		return static_cast<int>(flags);
	});
	QMetaType::registerConverter<int, TestClass::TestFlags>([](int flags) {
		return static_cast<TestClass::TestFlags>(flags);
	});
}

QJsonTypeConverter *EnumConverterTest::converter()
{
	return &_converter;
}

void EnumConverterTest::addConverterData()
{
	QTest::newRow("enum") << static_cast<int>(QJsonTypeConverter::Low)
						  << QList<QJsonValue::Type>{QJsonValue::Double, QJsonValue::String};
}

void EnumConverterTest::addMetaData()
{
	QTest::newRow("enum") << qMetaTypeId<TestClass::TestEnum>()
						  << true;
	QTest::newRow("flags") << qMetaTypeId<TestClass::TestFlags>()
						   << true;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::Int)
							 << false;
}

void EnumConverterTest::addCommonSerData()
{
	QTest::newRow("enum.int") << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << qMetaTypeId<TestClass::TestEnum>()
							  << QVariant::fromValue(TestClass::TestEnum::Key1)
							  << QJsonValue{static_cast<int>(TestClass::TestEnum::Key1)};

	QTest::newRow("enum.string") << QVariantHash{{QStringLiteral("enumAsString"), true}}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << qMetaTypeId<TestClass::TestEnum>()
								 << QVariant::fromValue(TestClass::TestEnum::Key4)
								 << QJsonValue{QStringLiteral("Key4")};

	QTest::newRow("flags.int") << QVariantHash{}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<TestClass::TestFlags>()
							   << QVariant::fromValue(TestClass::TestFlag::Flag2 | TestClass::TestFlag::Flag4)
							   << QJsonValue{static_cast<int>(TestClass::TestFlag::Flag2 | TestClass::TestFlag::Flag4)};

	QTest::newRow("flags.string.simple") << QVariantHash{{QStringLiteral("enumAsString"), true}}
										 << TestQ{}
										 << static_cast<QObject*>(nullptr)
										 << qMetaTypeId<TestClass::TestFlags>()
										 << QVariant::fromValue<TestClass::TestFlags>(TestClass::TestFlag::Flag1)
										 << QJsonValue{QStringLiteral("Flag1")};

	QTest::newRow("flags.string.complex") << QVariantHash{{QStringLiteral("enumAsString"), true}}
										  << TestQ{}
										  << static_cast<QObject*>(nullptr)
										  << qMetaTypeId<TestClass::TestFlags>()
										  << QVariant::fromValue(TestClass::TestFlag::Flag2 | TestClass::TestFlag::Flag4)
										  << QJsonValue{QStringLiteral("Flag2|Flag4")};
}

QTEST_MAIN(EnumConverterTest)

#include "tst_enumconverter.moc"
