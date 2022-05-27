#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/stdtupleconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

using TestTpl1 = std::tuple<int, bool, double>;
using TestTpl2 = std::tuple<QList<int>, QPair<bool, bool>, QMap<QString, double>>;
using TestTpl3 = std::tuple<OpaqueDummy>;

Q_DECLARE_METATYPE(TestTpl1)
Q_DECLARE_METATYPE(TestTpl2)
Q_DECLARE_METATYPE(TestTpl3)

class TupleConverterTest : public TypeConverterTestBase
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
	StdTupleConverter _converter;
};

void TupleConverterTest::initTest()
{
	JsonSerializer::registerTupleConverters<int, bool, double>();
	JsonSerializer::registerTupleConverters<QList<int>, QPair<bool, bool>, QMap<QString, double>>();
	JsonSerializer::registerTupleConverters<OpaqueDummy>();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QMetaType::registerEqualsComparator<TestTpl1>();
#endif
}

TypeConverter *TupleConverterTest::converter()
{
	return &_converter;
}

void TupleConverterTest::addConverterData()
{
	QTest::newRow("tuple") << static_cast<int>(TypeConverter::Standard);
}

void TupleConverterTest::addMetaData()
{
	QTest::newRow("basic") << qMetaTypeId<std::tuple<int, bool, double>>()
						   << static_cast<QCborTag>(CborSerializer::Tuple)
						   << QCborValue::Array
						   << true
						   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("extended") << qMetaTypeId<std::tuple<QList<int>, QPair<bool, bool>, QMap<QString, double>>>()
							  << static_cast<QCborTag>(CborSerializer::NoTag)
							  << QCborValue::Array
							  << true
							  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QVariant)
							 << static_cast<QCborTag>(CborSerializer::NoTag)
							 << QCborValue::Null
							 << false
							 << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("wrongtag") << qMetaTypeId<std::tuple<int, bool, double>>()
							  << static_cast<QCborTag>(CborSerializer::Homogeneous)
							  << QCborValue::Array
							  << true
							  << TypeConverter::DeserializationCapabilityResult::WrongTag;
}

void TupleConverterTest::addCommonSerData()
{
	QTest::newRow("basic") << QVariantHash{}
						   << TestQ{{QMetaType::Int, 5, 1}, {QMetaType::Bool, true, 2}, {QMetaType::Double, 5.5, 3}}
						   << static_cast<QObject*>(this)
						   << qMetaTypeId<std::tuple<int, bool, double>>()
						   << QVariant::fromValue(std::make_tuple(5, true, 5.5))
						   << QCborValue{static_cast<QCborTag>(CborSerializer::Tuple), QCborArray{1, 2, 3}}
						   << QJsonValue{QJsonArray{1, 2, 3}};
}

void TupleConverterTest::addSerData()
{
	QTest::newRow("unconvertible") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<std::tuple<OpaqueDummy>>()
								   << QVariant::fromValue(std::tuple<OpaqueDummy>{})
								   << QCborValue{}
								   << QJsonValue{QJsonValue::Undefined};
}

void TupleConverterTest::addDeserData()
{
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << qMetaTypeId<TestTpl1>()
							 << QVariant{}
							 << QCborValue{static_cast<QCborTag>(CborSerializer::Tuple), QCborArray{1, true, 3.4, 4}}
							 << QJsonValue{QJsonArray{1, true, 3.4, 4}};
}

QTEST_MAIN(TupleConverterTest)

#include "tst_tupleconverter.moc"
