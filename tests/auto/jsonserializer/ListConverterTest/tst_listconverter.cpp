#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonlistconverter_p.h>

class ListConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;

private:
	QJsonListConverter _converter;
};

void ListConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<QList<int>>();
}

QJsonTypeConverter *ListConverterTest::converter()
{
	return &_converter;
}

void ListConverterTest::addConverterData()
{
	QTest::newRow("list") << static_cast<int>(QJsonTypeConverter::Standard)
						  << QList<QJsonValue::Type>{QJsonValue::Array};
}

void ListConverterTest::addMetaData()
{
	QTest::newRow("int") << qMetaTypeId<QList<int>>()
						 << true;
	QTest::newRow("string") << static_cast<int>(QMetaType::QStringList)
							<< true;
	QTest::newRow("variant") << static_cast<int>(QMetaType::QVariantList)
							 << true;
	QTest::newRow("object") << qMetaTypeId<QList<QObject*>>()
							<< true;
	QTest::newRow("list") << qMetaTypeId<QList<QList<bool>>>()
						  << true;
	QTest::newRow("pair") << qMetaTypeId<QList<QPair<int, bool>>>()
						  << true;
	QTest::newRow("invalid") << qMetaTypeId<QVector<int>>()
							 << false;
}

void ListConverterTest::addCommonSerData()
{

	QTest::newRow("empty") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<QList<int>>()
						   << QVariant::fromValue(QList<int>{})
						   << QJsonValue{QJsonArray{}};
	QTest::newRow("filled") << QVariantHash{}
							<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							<< static_cast<QObject*>(this)
							<< qMetaTypeId<QList<int>>()
							<< QVariant::fromValue(QList<int>{1, 3, 5})
							<< QJsonValue{QJsonArray{2, 4, 6}};
	QTest::newRow("string") << QVariantHash{}
							<< TestQ{{QMetaType::QString, QStringLiteral("test"), QStringLiteral("tree")}}
							<< static_cast<QObject*>(this)
							<< static_cast<int>(QMetaType::QStringList)
							<< QVariant{QStringList{QStringLiteral("test")}}
							<< QJsonValue{QJsonArray{QStringLiteral("tree")}};
	QTest::newRow("variant") << QVariantHash{}
							 << TestQ{{QMetaType::UnknownType, true, false}}
							 << static_cast<QObject*>(this)
							 << static_cast<int>(QMetaType::QVariantList)
							 << QVariant{QVariantList{true}}
							 << QJsonValue{QJsonArray{false}};
}

void ListConverterTest::addSerData()
{
	QTest::newRow("unconvertible") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<QList<OpaqueDummy>>()
								   << QVariant::fromValue(QList<OpaqueDummy>{{}, {}, {}})
								   << QJsonValue{QJsonValue::Undefined};
}

QTEST_MAIN(ListConverterTest)

#include "tst_listconverter.moc"
