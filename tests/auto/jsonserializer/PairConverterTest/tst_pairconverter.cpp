#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/pairconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class PairConverterTest : public TypeConverterTestBase
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
	PairConverter _converter;
};

void PairConverterTest::initTest()
{
	JsonSerializer::registerPairConverters<QString, int>();
	JsonSerializer::registerPairConverters<int, int>();
	JsonSerializer::registerPairConverters<bool, int>();
	JsonSerializer::registerPairConverters<QList<int>, QList<int>>();
	JsonSerializer::registerPairConverters<QPair<int, int>, QPair<bool, bool>>();
	JsonSerializer::registerPairConverters<QMap<int, int>, QList<int>>();

	QMetaType::registerEqualsComparator<QPair<bool, int>>();
	QMetaType::registerEqualsComparator<std::pair<bool, int>>();
	QMetaType::registerEqualsComparator<QPair<QMap<int, int>, QList<int>>>();
	QMetaType::registerEqualsComparator<QMap<int, int>>();
	QMetaType::registerEqualsComparator<QList<int>>();
}

TypeConverter *PairConverterTest::converter()
{
	return &_converter;
}

void PairConverterTest::addConverterData()
{
	QTest::newRow("pair") << static_cast<int>(TypeConverter::Standard);
}

void PairConverterTest::addMetaData()
{
	QTest::newRow("qt.tagged") << qMetaTypeId<QPair<QString, int>>()
							   << static_cast<QCborTag>(CborSerializer::Pair)
							   << QCborValue::Array
							   << true
							   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("qt.untagged") << qMetaTypeId<QPair<int, int>>()
								 << static_cast<QCborTag>(CborSerializer::NoTag)
								 << QCborValue::Array
								 << true
								 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("qt.list") << qMetaTypeId<QPair<QList<int>, QList<int>>>()
							 << static_cast<QCborTag>(CborSerializer::Pair)
							 << QCborValue::Array
							 << true
							 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("qt.pair") << qMetaTypeId<QPair<QPair<int, int>, QPair<bool, bool>>>()
							 << static_cast<QCborTag>(CborSerializer::Pair)
							 << QCborValue::Array
							 << true
							 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("qt.map") << qMetaTypeId<QPair<QMap<int, int>, QList<int>>>()
							<< static_cast<QCborTag>(CborSerializer::Pair)
							<< QCborValue::Array
							<< true
							<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("std.tagged") << qMetaTypeId<std::pair<QString, int>>()
								<< static_cast<QCborTag>(CborSerializer::Pair)
								<< QCborValue::Array
								<< true
								<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("std.untagged") << qMetaTypeId<std::pair<int, int>>()
								  << static_cast<QCborTag>(CborSerializer::NoTag)
								  << QCborValue::Array
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QVariantList)
								  << static_cast<QCborTag>(CborSerializer::NoTag)
								  << QCborValue::Array
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << qMetaTypeId<std::pair<int, int>>()
								 << static_cast<QCborTag>(CborSerializer::Homogeneous)
								 << QCborValue::Array
								 << true
								 << TypeConverter::DeserializationCapabilityResult::WrongTag;
}

void PairConverterTest::addCommonSerData()
{
	QTest::newRow("qt.basic") << QVariantHash{}
							  << TestQ{{QMetaType::Bool, true, 42}, {QMetaType::Int, 42, true}}
							  << static_cast<QObject*>(this)
							  << qMetaTypeId<QPair<bool, int>>()
							  << QVariant::fromValue(QPair<bool, int>{true, 42})
							  << QCborValue{static_cast<QCborTag>(CborSerializer::Pair), QCborArray{42, true}}
							  << QJsonValue{QJsonArray{42, true}};
	QTest::newRow("qt.advanced") << QVariantHash{}
								 << TestQ{
										{qMetaTypeId<QMap<int, int>>(), QVariant::fromValue(QMap<int, int>{{3, 7}}), 1},
										{qMetaTypeId<QList<int>>(), QVariant::fromValue(QList<int>{1, 2, 3}), 2}
									}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<QPair<QMap<int, int>, QList<int>>>()
								 << QVariant::fromValue(QPair<QMap<int, int>, QList<int>>{{{3, 7}}, {1, 2, 3}})
								 << QCborValue{static_cast<QCborTag>(CborSerializer::Pair), QCborArray{1, 2}}
								 << QJsonValue{QJsonArray{1, 2}};
	QTest::newRow("std") << QVariantHash{}
						 << TestQ{{QMetaType::Bool, true, 42}, {QMetaType::Int, 42, true}}
						 << static_cast<QObject*>(this)
						 << qMetaTypeId<std::pair<bool, int>>()
						 << QVariant::fromValue(std::pair<bool, int>{true, 42})
						 << QCborValue{static_cast<QCborTag>(CborSerializer::Pair), QCborArray{42, true}}
						 << QJsonValue{QJsonArray{42, true}};
}

void PairConverterTest::addSerData()
{
	QTest::newRow("qt.unconvertible") << QVariantHash{}
									  << TestQ{}
									  << static_cast<QObject*>(nullptr)
									  << qMetaTypeId<QPair<int, OpaqueDummy>>()
									  << QVariant::fromValue(QPair<int, OpaqueDummy>{})
									  << QCborValue{}
									  << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("std.unconvertible") << QVariantHash{}
									   << TestQ{}
									   << static_cast<QObject*>(nullptr)
									   << qMetaTypeId<std::pair<int, OpaqueDummy>>()
									   << QVariant::fromValue(std::pair<int, OpaqueDummy>{})
									   << QCborValue{}
									   << QJsonValue{QJsonValue::Undefined};
}

void PairConverterTest::addDeserData()
{
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << qMetaTypeId<QPair<bool, int>>()
							 << QVariant{}
							 << QCborValue{static_cast<QCborTag>(CborSerializer::Pair), QCborArray{true, 2, 3}}
							 << QJsonValue{QJsonArray{true, 2, 3}};
}

QTEST_MAIN(PairConverterTest)

#include "tst_pairconverter.moc"
