#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/stdoptionalconverter_p.h>

#include <optional>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

using ExtendedOptional = std::optional<std::pair<int, bool>>;

Q_DECLARE_METATYPE(std::optional<int>)
Q_DECLARE_METATYPE(ExtendedOptional)
Q_DECLARE_METATYPE(std::optional<OpaqueDummy>)

class OptionalConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;

private:
	StdOptionalConverter _converter;
};

void OptionalConverterTest::initTest()
{
	JsonSerializer::registerOptionalConverters<int>();
	JsonSerializer::registerOptionalConverters<std::pair<int, bool>>();
	JsonSerializer::registerOptionalConverters<OpaqueDummy>();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QMetaType::registerEqualsComparator<std::optional<int>>();
#endif
}

TypeConverter *OptionalConverterTest::converter()
{
	return &_converter;
}

void OptionalConverterTest::addConverterData()
{
	QTest::newRow("optional") << static_cast<int>(TypeConverter::Standard);
}

void OptionalConverterTest::addMetaData()
{
	QTest::newRow("basic") << qMetaTypeId<std::optional<int>>()
						   << static_cast<QCborTag>(CborSerializer::NoTag)
						   << QCborValue::Double
						   << true
						   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("extended") << qMetaTypeId<std::optional<std::pair<int, bool>>>()
							  << static_cast<QCborTag>(CborSerializer::NoTag)
							  << QCborValue::Null
							  << true
							  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("invalid") << qMetaTypeId<QList<int>>()
							 << static_cast<QCborTag>(CborSerializer::NoTag)
							 << QCborValue::Integer
							 << false
							 << TypeConverter::DeserializationCapabilityResult::Negative;
}

void OptionalConverterTest::addCommonSerData()
{
	QTest::newRow("basic") << QVariantHash{}
						   << TestQ{{QMetaType::Int, 5, 1}}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<std::optional<int>>()
						   << QVariant::fromValue(std::optional<int>{5})
						   << QCborValue{1}
						   << QJsonValue{1};
	QTest::newRow("nullopt") << QVariantHash{}
							 << TestQ{{QMetaType::Nullptr, QVariant::fromValue(nullptr), QJsonValue::Null}}
							 << static_cast<QObject*>(nullptr)
							 << qMetaTypeId<std::optional<int>>()
							 << QVariant::fromValue<std::optional<int>>(std::nullopt)
							 << QCborValue{QCborValue::Null}
							 << QJsonValue{QJsonValue::Null};
}

void OptionalConverterTest::addSerData()
{
	QTest::newRow("invalid.unconvertible") << QVariantHash{}
										   << TestQ{}
										   << static_cast<QObject*>(nullptr)
										   << qMetaTypeId<std::optional<OpaqueDummy>>()
										   << QVariant::fromValue(std::optional<OpaqueDummy>{OpaqueDummy{}})
										   << QCborValue{}
										   << QJsonValue{QJsonValue::Undefined};
}

QTEST_MAIN(OptionalConverterTest)

#include "tst_optionalconverter.moc"
