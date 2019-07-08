#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonstdvariantconverter_p.h>

#include <variant>

using TestVar1 = std::variant<int, bool, double>;
using TestVar2 = std::variant<QList<int>, QPair<bool, bool>, QMap<QString, double>>;
using TestVar3 = std::variant<OpaqueDummy>;

Q_DECLARE_METATYPE(TestVar1)
Q_DECLARE_METATYPE(TestVar2)
Q_DECLARE_METATYPE(TestVar3)

class VariantConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;
	void addDeserData() override;

private:
	QJsonStdVariantConverter _converter;
};



void VariantConverterTest::initTest()
{
	QJsonSerializer::registerVariantConverters<int, bool, double>("std::variant<int, bool, double>");
	QJsonSerializer::registerVariantConverters<QList<int>, QPair<bool, bool>, QMap<QString, double>>("std::variant<QList<int>, QPair<bool, bool>, QMap<QString, double>>");

	QMetaType::registerEqualsComparator<TestVar1>();
}

QJsonTypeConverter *VariantConverterTest::converter()
{
	return &_converter;
}

void VariantConverterTest::addConverterData()
{
	QTest::newRow("variant") << static_cast<int>(QJsonTypeConverter::Standard)
							 << QList<QJsonValue::Type>{
									QJsonValue::Null,
									QJsonValue::Bool,
									QJsonValue::Double,
									QJsonValue::String,
									QJsonValue::Array,
									QJsonValue::Object
								};
}

void VariantConverterTest::addMetaData()
{
	QTest::newRow("basic") << qMetaTypeId<std::variant<int, bool, double>>()
						   << true;
	QTest::newRow("extended") << qMetaTypeId<std::variant<QList<int>, QPair<bool, bool>, QMap<QString, double>>>()
							  << true;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QVariant)
							 << false;
}

void VariantConverterTest::addCommonSerData()
{
	QTest::newRow("basic.int") << QVariantHash{}
							   << TestQ{{QMetaType::Int, 5, 1}}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<std::variant<int, bool, double>>()
							   << QVariant::fromValue<std::variant<int, bool, double>>(5)
							   << QJsonValue{1};
	QTest::newRow("basic.bool") << QVariantHash{}
								<< TestQ{{QMetaType::Bool, true, 2}}
								<< static_cast<QObject*>(nullptr)
								<< qMetaTypeId<std::variant<int, bool, double>>()
								<< QVariant::fromValue<std::variant<int, bool, double>>(true)
								<< QJsonValue{2};
	QTest::newRow("basic.double") << QVariantHash{}
								  << TestQ{{QMetaType::Double, 7.3, 3}}
								  << static_cast<QObject*>(nullptr)
								  << qMetaTypeId<std::variant<int, bool, double>>()
								  << QVariant::fromValue<std::variant<int, bool, double>>(7.3)
								  << QJsonValue{3};
}

void VariantConverterTest::addSerData()
{
	QTest::newRow("invalid.unconvertible") << QVariantHash{}
										   << TestQ{}
										   << static_cast<QObject*>(nullptr)
										   << qMetaTypeId<std::variant<OpaqueDummy>>()
										   << QVariant::fromValue<std::variant<OpaqueDummy>>(OpaqueDummy{})
										   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("invalid.input") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<std::variant<int, bool, double>>()
								   << QVariant::fromValue(QStringLiteral("Hello World"))
								   << QJsonValue{QJsonValue::Undefined};
}

void VariantConverterTest::addDeserData()
{
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << qMetaTypeId<std::variant<int, bool, double>>()
							 << QVariant{}
							 << QJsonValue{QStringLiteral("Hello World")};
}

QTEST_MAIN(VariantConverterTest)

#include "tst_variantconverter.moc"
