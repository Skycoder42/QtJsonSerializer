#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/stdvariantconverter_p.h>

#include <variant>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

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
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;
	void addDeserData() override;

private:
	StdVariantConverter _converter;
};



void VariantConverterTest::initTest()
{
	JsonSerializer::registerVariantConverters<int, bool, double>();
	JsonSerializer::registerVariantConverters<QList<int>, QPair<bool, bool>, QMap<QString, double>>();
	JsonSerializer::registerVariantConverters<OpaqueDummy>();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QMetaType::registerEqualsComparator<TestVar1>();
#endif
}

TypeConverter *VariantConverterTest::converter()
{
	return &_converter;
}

void VariantConverterTest::addConverterData()
{
	QTest::newRow("variant") << static_cast<int>(TypeConverter::Standard);
}

void VariantConverterTest::addMetaData()
{
	QTest::newRow("basic") << qMetaTypeId<std::variant<int, bool, double>>()
						   << static_cast<QCborTag>(CborSerializer::NoTag)
						   << QCborValue::Double
						   << true
						   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("extended") << qMetaTypeId<std::variant<QList<int>, QPair<bool, bool>, QMap<QString, double>>>()
							  << static_cast<QCborTag>(CborSerializer::NoTag)
							  << QCborValue::Array
							  << true
							  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QVariant)
							 << static_cast<QCborTag>(CborSerializer::NoTag)
							 << QCborValue::Null
							 << false
							 << TypeConverter::DeserializationCapabilityResult::Negative;
}

void VariantConverterTest::addCommonSerData()
{
	QTest::newRow("basic.int") << QVariantHash{}
							   << TestQ{{QMetaType::Int, 5, 1}}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<std::variant<int, bool, double>>()
							   << QVariant::fromValue<std::variant<int, bool, double>>(5)
							   << QCborValue{1}
							   << QJsonValue{1};
	QTest::newRow("basic.bool") << QVariantHash{}
								<< TestQ{{QMetaType::Bool, true, 2}}
								<< static_cast<QObject*>(nullptr)
								<< qMetaTypeId<std::variant<int, bool, double>>()
								<< QVariant::fromValue<std::variant<int, bool, double>>(true)
								<< QCborValue{2}
								<< QJsonValue{2};
	QTest::newRow("basic.double") << QVariantHash{}
								  << TestQ{{QMetaType::Double, 7.3, 3}}
								  << static_cast<QObject*>(nullptr)
								  << qMetaTypeId<std::variant<int, bool, double>>()
								  << QVariant::fromValue<std::variant<int, bool, double>>(7.3)
								  << QCborValue{3}
								  << QJsonValue{3};
}

void VariantConverterTest::addSerData()
{
	QTest::newRow("invalid.unconvertible") << QVariantHash{}
										   << TestQ{}
										   << static_cast<QObject*>(nullptr)
										   << qMetaTypeId<std::variant<OpaqueDummy>>()
										   << QVariant::fromValue<std::variant<OpaqueDummy>>(OpaqueDummy{})
										   << QCborValue{}
										   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("invalid.input") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<std::variant<int, bool, double>>()
								   << QVariant::fromValue(QStringLiteral("Hello World"))
								   << QCborValue{}
								   << QJsonValue{QJsonValue::Undefined};
}

void VariantConverterTest::addDeserData()
{
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << qMetaTypeId<std::variant<int, bool, double>>()
							 << QVariant{}
							 << QCborValue{QStringLiteral("Hello World")}
							 << QJsonValue{QStringLiteral("Hello World")};
}

QTEST_MAIN(VariantConverterTest)

#include "tst_variantconverter.moc"
