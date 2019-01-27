#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonmultimapconverter_p.h>

template<template <typename, typename> class TContainer>
bool tCompare(const TContainer<QString, int> &lhs, const TContainer<QString, int> &rhs) {
	if(lhs.size() != rhs.size())
		return false;
	if(lhs.isEmpty())
		return true;

	for(auto it = lhs.constBegin(); it != lhs.constEnd(); it++) {
		auto fIt = std::find_if(rhs.keyValueBegin(), rhs.keyValueEnd(), [it](const std::pair<QString, int> &v){
			return it.key() == v.first &&
					it.value() == v.second;
		});
		if(fIt != rhs.keyValueEnd())
			return true;
	}
	return false;
}

// create operators with higher priority to be used by qvariant compare
bool operator==(const QMultiMap<QString, int> &lhs, const QMultiMap<QString, int> &rhs) {
	return tCompare(lhs, rhs);
}

bool operator==(const QMultiHash<QString, int> &lhs, const QMultiHash<QString, int> &rhs) {
	return tCompare(lhs, rhs);
}

class MultiMapConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	bool compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line) override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;
	void addDeserData() override;

private:
	QJsonMultiMapConverter _converter;
};

void MultiMapConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<QMultiMap<QString, int>>();
	QMetaType::registerEqualsComparator<QMultiHash<QString, int>>();
}

bool MultiMapConverterTest::compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line)
{
	return TypeConverterTestBase::compare(type, actual, expected, aName, eName, file, line);
}

QJsonTypeConverter *MultiMapConverterTest::converter()
{
	return &_converter;
}

void MultiMapConverterTest::addConverterData()
{
	QTest::newRow("multimap") << static_cast<int>(QJsonTypeConverter::Standard)
							  << QList<QJsonValue::Type>{QJsonValue::Object, QJsonValue::Array};
}

void MultiMapConverterTest::addMetaData()
{
	QTest::newRow("int") << qMetaTypeId<QMultiMap<QString, int>>()
						 << true;
	QTest::newRow("string") << qMetaTypeId<QMultiMap<QString, QString>>()
							<< true;
	QTest::newRow("object") << qMetaTypeId<QMultiMap<QString, QObject*>>()
							<< true;
	QTest::newRow("list") << qMetaTypeId<QMultiMap<QString, QMap<QString, bool>>>()
						  << true;
	QTest::newRow("pair") << qMetaTypeId<QMultiMap<QString, QPair<int, bool>>>()
						  << true;

	QTest::newRow("hash") << qMetaTypeId<QMultiHash<QString, int>>()
						  << true;

	QTest::newRow("invalid.pair") << qMetaTypeId<QPair<QString, int>>()
								  << false;
	QTest::newRow("invalid.key") << qMetaTypeId<QMultiMap<int, int>>()
								 << false;
}

void MultiMapConverterTest::addCommonSerData()
{
	QTest::newRow("empty.obj") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<QMultiMap<QString, int>>()
							   << QVariant::fromValue(QMultiMap<QString, int>{})
							   << QJsonValue{QJsonObject{}};
	QTest::newRow("empty.arr") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::List)}}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<QMultiMap<QString, int>>()
							   << QVariant::fromValue(QMultiMap<QString, int>{})
							   << QJsonValue{QJsonArray{}};
	QTest::newRow("filled.map.obj") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
									<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
									<< static_cast<QObject*>(this)
									<< qMetaTypeId<QMultiMap<QString, int>>()
									<< QVariant::fromValue(QMultiMap<QString, int>{
											  {QStringLiteral("a"), 1},
											  {QStringLiteral("a"), 3},
											  {QStringLiteral("b"), 5}
										  })
									<< QJsonValue{QJsonObject{
											{QStringLiteral("a"), QJsonArray{2, 4}},
											{QStringLiteral("b"), QJsonArray{6}}
										}};
	QTest::newRow("filled.map.arr") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::List)}}
									<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
									<< static_cast<QObject*>(this)
									<< qMetaTypeId<QMultiMap<QString, int>>()
									<< QVariant::fromValue(QMultiMap<QString, int>{
											  {QStringLiteral("a"), 1},
											  {QStringLiteral("a"), 3},
											  {QStringLiteral("b"), 5}
										  })
									<< QJsonValue{QJsonArray{
											QJsonArray{QStringLiteral("a"), 2},
											QJsonArray{QStringLiteral("a"), 4},
											QJsonArray{QStringLiteral("b"), 6}
										}};
	QTest::newRow("filled.hash.obj") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
									 << TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
									 << static_cast<QObject*>(this)
									 << qMetaTypeId<QMultiHash<QString, int>>()
									 << QVariant::fromValue(QMultiHash<QString, int>{
											   {QStringLiteral("a"), 1},
											   {QStringLiteral("a"), 3},
											   {QStringLiteral("b"), 5}
										   })
									 << QJsonValue{QJsonObject{
											 {QStringLiteral("a"), QJsonArray{2, 4}},
											 {QStringLiteral("b"), QJsonArray{6}}
										 }};
	QTest::newRow("filled.hash.arr") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::List)}}
									 << TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
									 << static_cast<QObject*>(this)
									 << qMetaTypeId<QMultiHash<QString, int>>()
									 << QVariant::fromValue(QMultiHash<QString, int>{
											   {QStringLiteral("a"), 1},
											   {QStringLiteral("a"), 3},
											   {QStringLiteral("b"), 5}
										   })
									 << QJsonValue{QJsonArray{
											 QJsonArray{QStringLiteral("a"), 2},
											 QJsonArray{QStringLiteral("a"), 4},
											 QJsonArray{QStringLiteral("b"), 6}
										 }};
}

void MultiMapConverterTest::addSerData()
{
	QTest::newRow("unconvertible") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<QMultiMap<QString, OpaqueDummy>>()
								   << QVariant::fromValue(QMap<QString, OpaqueDummy>{{QStringLiteral("d"), {}}})
								   << QJsonValue{QJsonValue::Undefined};
}

void MultiMapConverterTest::addDeserData()
{
	QTest::newRow("map.obj.single") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
									<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
									<< static_cast<QObject*>(this)
									<< qMetaTypeId<QMultiHash<QString, int>>()
									<< QVariant::fromValue(QMultiHash<QString, int>{
											  {QStringLiteral("a"), 1},
											  {QStringLiteral("a"), 3},
											  {QStringLiteral("b"), 5}
										  })
									<< QJsonValue{QJsonObject{
											{QStringLiteral("a"), QJsonArray{2, 4}},
											{QStringLiteral("b"), 6}
										}};
	QTest::newRow("unmatchable") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::List)}}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << qMetaTypeId<QMultiMap<QString, OpaqueDummy>>()
								 << QVariant{}
								 << QJsonValue{QJsonArray{
										 QJsonArray{QStringLiteral("a"), 4, false}
									 }};
}

QTEST_MAIN(MultiMapConverterTest)

#include "tst_multimapconverter.moc"
