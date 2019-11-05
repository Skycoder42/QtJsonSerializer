#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonstdtupleconverter_p.h>
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
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;
	void addDeserData() override;

private:
	QJsonStdTupleConverter _converter;
};

void TupleConverterTest::initTest()
{
	QJsonSerializer::registerTupleConverters<int, bool, double>();
	QJsonSerializer::registerTupleConverters<QList<int>, QPair<bool, bool>, QMap<QString, double>>();
	QJsonSerializer::registerTupleConverters<OpaqueDummy>();

	QMetaType::registerEqualsComparator<TestTpl1>();
}

QJsonTypeConverter *TupleConverterTest::converter()
{
	return &_converter;
}

void TupleConverterTest::addConverterData()
{
	QTest::newRow("tuple") << static_cast<int>(QJsonTypeConverter::Standard);
}

void TupleConverterTest::addMetaData()
{
	QTest::newRow("basic") << qMetaTypeId<std::tuple<int, bool, double>>()
						   << static_cast<QCborTag>(QCborSerializer::Tuple)
						   << QCborValue::Array
						   << true
						   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("extended") << qMetaTypeId<std::tuple<QList<int>, QPair<bool, bool>, QMap<QString, double>>>()
							  << static_cast<QCborTag>(QCborSerializer::NoTag)
							  << QCborValue::Array
							  << true
							  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QVariant)
							 << static_cast<QCborTag>(QCborSerializer::NoTag)
							 << QCborValue::Null
							 << false
							 << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("wrongtag") << qMetaTypeId<std::tuple<int, bool, double>>()
							  << static_cast<QCborTag>(QCborSerializer::Homogeneous)
							  << QCborValue::Array
							  << true
							  << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
}

void TupleConverterTest::addCommonSerData()
{
	QTest::newRow("basic") << QVariantHash{}
						   << TestQ{{QMetaType::Int, 5, 1}, {QMetaType::Bool, true, 2}, {QMetaType::Double, 5.5, 3}}
						   << static_cast<QObject*>(this)
						   << qMetaTypeId<std::tuple<int, bool, double>>()
						   << QVariant::fromValue(std::make_tuple(5, true, 5.5))
						   << QCborValue{static_cast<QCborTag>(QCborSerializer::Tuple), QCborArray{1, 2, 3}}
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
							 << QCborValue{static_cast<QCborTag>(QCborSerializer::Tuple), QCborArray{1, true, 3.4, 4}}
							 << QJsonValue{QJsonArray{1, true, 3.4, 4}};
}

QTEST_MAIN(TupleConverterTest)

#include "tst_tupleconverter.moc"
