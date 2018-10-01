#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonpairconverter_p.h>

class PairConverterTest : public TypeConverterTestBase
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
	QJsonPairConverter _converter;
};

void PairConverterTest::initTest()
{
	QJsonSerializer::registerPairConverters<bool, int>();
	QJsonSerializer::registerPairConverters<int, QList<int>>();

	QMetaType::registerEqualsComparator<QPair<bool, int>>();
	QMetaType::registerEqualsComparator<std::pair<bool, int>>();
	QMetaType::registerEqualsComparator<QPair<int, QList<int>>>();
}

QJsonTypeConverter *PairConverterTest::converter()
{
	return &_converter;
}

void PairConverterTest::addConverterData()
{
	QTest::newRow("pair") << static_cast<int>(QJsonTypeConverter::Standard)
						  << QList<QJsonValue::Type>{QJsonValue::Array};
}

void PairConverterTest::addMetaData()
{
	QTest::newRow("qt.basic") << qMetaTypeId<QPair<QString, int>>()
							  << true;
	QTest::newRow("qt.double") << qMetaTypeId<QPair<int, int>>()
							   << true;
	QTest::newRow("qt.list") << qMetaTypeId<QPair<QList<int>, QList<int>>>()
							 << true;
	QTest::newRow("qt.pair") << qMetaTypeId<QPair<QPair<int, int>, QPair<bool, bool>>>()
							 << true;
	QTest::newRow("qt.map") << qMetaTypeId<QPair<QMap<QString, int>, QList<int>>>()
							<< true;
	QTest::newRow("std.basic") << qMetaTypeId<std::pair<QString, int>>()
							   << true;
	QTest::newRow("std.double") << qMetaTypeId<std::pair<int, int>>()
								<< true;
	QTest::newRow("std.list") << qMetaTypeId<std::pair<QList<int>, QList<int>>>()
							  << true;
	QTest::newRow("std.pair") << qMetaTypeId<std::pair<std::pair<int, int>, std::pair<bool, bool>>>()
							  << true;
	QTest::newRow("std.map") << qMetaTypeId<std::pair<QMap<QString, int>, QList<int>>>()
							 << true;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QVariantList)
							 << false;
}

void PairConverterTest::addCommonSerData()
{
	QTest::newRow("qt.basic") << QVariantHash{}
							  << TestQ{{QMetaType::Bool, true, 42}, {QMetaType::Int, 42, true}}
							  << static_cast<QObject*>(this)
							  << qMetaTypeId<QPair<bool, int>>()
							  << QVariant::fromValue(QPair<bool, int>{true, 42})
							  << QJsonValue{QJsonArray{42, true}};
	QTest::newRow("qt.advanced") << QVariantHash{}
								 << TestQ{
										{QMetaType::Int, 3, 1},
										{qMetaTypeId<QList<int>>(), QVariant::fromValue(QList<int>{}), 2}
									}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<QPair<int, QList<int>>>()
								 << QVariant::fromValue(QPair<int, QList<int>>{3, {}})
								 << QJsonValue{QJsonArray{1, 2}};
	QTest::newRow("std") << QVariantHash{}
						 << TestQ{{QMetaType::Bool, true, 42}, {QMetaType::Int, 42, true}}
						 << static_cast<QObject*>(this)
						 << qMetaTypeId<std::pair<bool, int>>()
						 << QVariant::fromValue(std::pair<bool, int>{true, 42})
						 << QJsonValue{QJsonArray{42, true}};
}

void PairConverterTest::addSerData()
{
	QTest::newRow("qt.unconvertible") << QVariantHash{}
									  << TestQ{}
									  << static_cast<QObject*>(nullptr)
									  << qMetaTypeId<QPair<int, OpaqueDummy>>()
									  << QVariant::fromValue(QPair<int, OpaqueDummy>{})
									  << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("std.unconvertible") << QVariantHash{}
									   << TestQ{}
									   << static_cast<QObject*>(nullptr)
									   << qMetaTypeId<std::pair<int, OpaqueDummy>>()
									   << QVariant::fromValue(std::pair<int, OpaqueDummy>{})
									   << QJsonValue{QJsonValue::Undefined};
}

void PairConverterTest::addDeserData()
{
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << qMetaTypeId<QPair<bool, int>>()
							 << QVariant{}
							 << QJsonValue{QJsonArray{true, 2, 3}};
}

QTEST_MAIN(PairConverterTest)

#include "tst_pairconverter.moc"
