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
	QMetaType::registerEqualsComparator<QLinkedList<int>>();
	QMetaType::registerEqualsComparator<QVector<int>>();
	QMetaType::registerEqualsComparator<QStack<int>>();
	QMetaType::registerEqualsComparator<QQueue<int>>();
	QMetaType::registerEqualsComparator<QSet<int>>();
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

	QTest::newRow("linkedList") << qMetaTypeId<QLinkedList<int>>()
								<< true;
	QTest::newRow("vector") << qMetaTypeId<QVector<int>>()
							<< true;
	QTest::newRow("stack") << qMetaTypeId<QStack<int>>()
						   << true;
	QTest::newRow("queue") << qMetaTypeId<QQueue<int>>()
						   << true;
	QTest::newRow("set") << qMetaTypeId<QSet<int>>()
						 << true;

	QTest::newRow("invalid") << qMetaTypeId<QPair<int, int>>()
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

	QTest::newRow("linkedList") << QVariantHash{}
								<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
								<< static_cast<QObject*>(this)
								<< qMetaTypeId<QLinkedList<int>>()
								<< QVariant::fromValue(QLinkedList<int>{1, 3, 5})
								<< QJsonValue{QJsonArray{2, 4, 6}};
	QTest::newRow("vector") << QVariantHash{}
							<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							<< static_cast<QObject*>(this)
							<< qMetaTypeId<QVector<int>>()
							<< QVariant::fromValue(QVector<int>{1, 3, 5})
							<< QJsonValue{QJsonArray{2, 4, 6}};

	{
		QStack<int> s;
		s.push(1);
		s.push(3);
		s.push(5);
		QTest::newRow("stack") << QVariantHash{}
							   << TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							   << static_cast<QObject*>(this)
							   << qMetaTypeId<QStack<int>>()
							   << QVariant::fromValue(s)
							   << QJsonValue{QJsonArray{2, 4, 6}};
	}

	{
		QQueue<int> q;
		q.enqueue(1);
		q.enqueue(3);
		q.enqueue(5);
		QTest::newRow("queue") << QVariantHash{}
							   << TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							   << static_cast<QObject*>(this)
							   << qMetaTypeId<QQueue<int>>()
							   << QVariant::fromValue(q)
							   << QJsonValue{QJsonArray{2, 4, 6}};
	}

	{
		QSet<int> s{1, 3, 5};
		TestQ q;
		auto ctr = 0;
		for(const auto &v : qAsConst(s))
			q.append({QMetaType::Int, v, (++ctr)*2});
		QTest::newRow("set") << QVariantHash{}
							 << q
							 << static_cast<QObject*>(this)
							 << qMetaTypeId<QSet<int>>()
							 << QVariant::fromValue(s)
							 << QJsonValue{QJsonArray{2, 4, 6}};
	}
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
