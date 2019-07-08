#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonstdtupleconverter_p.h>

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
	QJsonSerializer_registerTupleConverters_named(int, bool, double);
	QJsonSerializer_registerTupleConverters_named(QList<int>, QPair<bool, bool>, QMap<QString, double>);

	QMetaType::registerEqualsComparator<TestTpl1>();
}

QJsonTypeConverter *TupleConverterTest::converter()
{
	return &_converter;
}

void TupleConverterTest::addConverterData()
{
	QTest::newRow("tuple") << static_cast<int>(QJsonTypeConverter::Standard)
						   << QList<QJsonValue::Type>{QJsonValue::Array};
}

void TupleConverterTest::addMetaData()
{
	QTest::newRow("basic") << qMetaTypeId<std::tuple<int, bool, double>>()
						   << true;
	QTest::newRow("extended") << qMetaTypeId<std::tuple<QList<int>, QPair<bool, bool>, QMap<QString, double>>>()
							  << true;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QVariantList)
							 << false;
}

void TupleConverterTest::addCommonSerData()
{
	QTest::newRow("basic") << QVariantHash{}
						   << TestQ{{QMetaType::Int, 5, 1}, {QMetaType::Bool, true, 2}, {QMetaType::Double, 5.5, 3}}
						   << static_cast<QObject*>(this)
						   << qMetaTypeId<std::tuple<int, bool, double>>()
						   << QVariant::fromValue(std::make_tuple(5, true, 5.5))
						   << QJsonValue{QJsonArray{1, 2, 3}};
}

void TupleConverterTest::addSerData()
{
	QTest::newRow("unconvertible") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<std::tuple<OpaqueDummy>>()
								   << QVariant::fromValue(std::tuple<OpaqueDummy>{})
								   << QJsonValue{QJsonValue::Undefined};
}

void TupleConverterTest::addDeserData()
{
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << qMetaTypeId<TestTpl1>()
							 << QVariant{}
							 << QJsonValue{QJsonArray{1, true, 3.4, 4}};
}

QTEST_MAIN(TupleConverterTest)

#include "tst_tupleconverter.moc"
