#include "testobject.h"

#include <QString>
#include <QtTest>
#include <QJsonSerializer>

class SerializerTest : public QObject
{
	Q_OBJECT

public:
	SerializerTest();

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testVariantConversions_data();
	void testVariantConversions();
	void testObjectSerialization_data();
	void testObjectSerialization();

private:
	QJsonSerializer *serializer;
};

SerializerTest::SerializerTest()
{
}

void SerializerTest::initTestCase()
{
	QJsonSerializer::registerListConverters<QList<int>>();
	QJsonSerializer::registerListConverters<TestObject*>();
	QJsonSerializer::registerListConverters<QList<TestObject*>>();
	serializer = new QJsonSerializer(this);
}

void SerializerTest::cleanupTestCase()
{
	serializer->deleteLater();
	serializer = nullptr;
}

void SerializerTest::testVariantConversions_data()
{
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<int>("targetType");

	QTest::newRow("QList<int>") << QVariant::fromValue<QList<int>>({3, 7, 13})
								<< (int)QVariant::List;
	QList<int> l1 = {0, 1, 2};
	QList<int> l2 = {3, 4, 5};
	QList<int> l3 = {6, 7, 8};
	QTest::newRow("QList<QList<int>>") << QVariant::fromValue<QList<QList<int>>>({l1, l2, l3})
									   << (int)QVariant::List;

	QTest::newRow("TestObject*") << QVariant::fromValue<TestObject*>(new TestObject(this))
								 << (int)QMetaType::QObjectStar;
	QTest::newRow("QList<TestObject*>") << QVariant::fromValue<QList<TestObject*>>({
																					   new TestObject(this),
																					   new TestObject(this),
																					   new TestObject(this)
																				   })
										<< (int)QVariant::List;
	QList<TestObject*> o1 = {
		new TestObject(this),
		new TestObject(this),
		new TestObject(this)
	};
	QList<TestObject*> o2 = {
		new TestObject(this),
		new TestObject(this),
		new TestObject(this)
	};
	QList<TestObject*> o3 = {
		new TestObject(this),
		new TestObject(this),
		new TestObject(this)
	};
	QTest::newRow("QList<QList<TestObject*>>") << QVariant::fromValue<QList<QList<TestObject*>>>({o1, o2, o3})
											   << (int)QVariant::List;
}

void SerializerTest::testVariantConversions()
{
	QFETCH(QVariant, data);
	QFETCH(int, targetType);

	auto origType = data.userType();
	auto convData = data;
	QVERIFY(convData.convert(targetType));
	QVERIFY(convData.convert(origType));
	QCOMPARE(convData, data);
}

void SerializerTest::testObjectSerialization_data()
{
	QTest::addColumn<TestObject*>("object");
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<bool>("works");

	QTest::newRow("default") << new TestObject(this)
							 << QJsonObject({
												{"intProperty", 0},
												{"boolProperty", false},
												{"stringProperty", QString()},
												{"doubleProperty", 0},
												{"simpeList", QJsonArray()},
												{"leveledList", QJsonArray()},
												{"childObject", QJsonValue::Null},
												{"simpleChildren", QJsonArray()},
												{"leveledChildren", QJsonArray()}
											})
							 << true;

	QTest::newRow("basic") << TestObject::createBasic(42, true, "baum", 4.2, this)
						   << QJsonObject({
											  {"intProperty", 42},
											  {"boolProperty", true},
											  {"stringProperty", QStringLiteral("baum")},
											  {"doubleProperty", 4.2},
											  {"simpeList", QJsonArray()},
											  {"leveledList", QJsonArray()},
											  {"childObject", QJsonValue::Null},
											  {"simpleChildren", QJsonArray()},
											  {"leveledChildren", QJsonArray()}
										  })
						   << true;

	QTest::newRow("list") << TestObject::createList({3, 7, 13}, {}, this)
						  << QJsonObject({
											 {"intProperty", 0},
											 {"boolProperty", false},
											 {"stringProperty", QString()},
											 {"doubleProperty", 0},
											 {"simpeList", QJsonArray({3, 7, 13})},
											 {"leveledList", QJsonArray()},
											 {"childObject", QJsonValue::Null},
											 {"simpleChildren", QJsonArray()},
											 {"leveledChildren", QJsonArray()}
										 })
						  << true;

	{
		QList<int> l1 = {0, 1, 2};
		QList<int> l2 = {3, 4, 5};
		QList<int> l3 = {6, 7, 8};
		QJsonArray j1 = {0, 1, 2};
		QJsonArray j2 = {3, 4, 5};
		QJsonArray j3 = {6, 7, 8};
		QTest::newRow("list<list>") << TestObject::createList({3, 7, 13}, {l1, l2, l3}, this)
									<< QJsonObject({
													   {"intProperty", 0},
													   {"boolProperty", false},
													   {"stringProperty", QString()},
													   {"doubleProperty", 0},
													   {"simpeList", QJsonArray({3, 7, 13})},
													   {"leveledList", QJsonArray({j1, j2, j3})},
													   {"childObject", QJsonValue::Null},
													   {"simpleChildren", QJsonArray()},
													   {"leveledChildren", QJsonArray()}
												   })
									<< true;
	}

	QTest::newRow("child") << TestObject::createChild(TestObject::createBasic(42, true, "baum", 4.2), {}, {}, this)
							 << QJsonObject({
												{"intProperty", 0},
												{"boolProperty", false},
												{"stringProperty", QString()},
												{"doubleProperty", 0},
												{"simpeList", QJsonArray()},
												{"leveledList", QJsonArray()},
												{"childObject", QJsonObject({
													 {"intProperty", 42},
													 {"boolProperty", true},
													 {"stringProperty", QStringLiteral("baum")},
													 {"doubleProperty", 4.2},
													 {"simpeList", QJsonArray()},
													 {"leveledList", QJsonArray()},
													 {"childObject", QJsonValue::Null},
													 {"simpleChildren", QJsonArray()},
													 {"leveledChildren", QJsonArray()}
												 })},
												{"simpleChildren", QJsonArray()},
												{"leveledChildren", QJsonArray()}
											})
							 << true;

	{
		auto c1 = TestObject::createBasic(1, false, "1", 1.1);
		auto c2 = TestObject::createBasic(2, true, "2", 2.2);
		auto c3 = TestObject::createBasic(3, false, "3", 3.3);
		QJsonObject j1({
						   {"intProperty", 1},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("1")},
						   {"doubleProperty", 1.1},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j2({
						   {"intProperty", 2},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("2")},
						   {"doubleProperty", 2.2},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j3({
						   {"intProperty", 3},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("3")},
						   {"doubleProperty", 3.3},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QTest::newRow("childlist") << TestObject::createChild(nullptr, {c1, c2, c3}, {}, this)
								   << QJsonObject({
													  {"intProperty", 0},
													  {"boolProperty", false},
													  {"stringProperty", QString()},
													  {"doubleProperty", 0},
													  {"simpeList", QJsonArray()},
													  {"leveledList", QJsonArray()},
													  {"childObject", QJsonValue::Null},
													  {"simpleChildren", QJsonArray({j1, j2, j3})},
													  {"leveledChildren", QJsonArray()}
												  })
								   << true;
	}

	{
		auto c0 = TestObject::createBasic(0, false, "0", 0.0);
		auto c1 = TestObject::createBasic(1, true, "1", 1.1);
		auto c2 = TestObject::createBasic(2, false, "2", 2.2);
		auto c3 = TestObject::createBasic(3, true, "3", 3.3);
		auto c4 = TestObject::createBasic(4, false, "4", 4.4);
		auto c5 = TestObject::createBasic(5, true, "5", 5.5);
		auto c6 = TestObject::createBasic(6, false, "6", 6.6);
		auto c7 = TestObject::createBasic(7, true, "7", 7.7);
		auto c8 = TestObject::createBasic(8, false, "8", 8.8);
		QList<QList<TestObject*>> ccl = {
			{c0, c1, c2},
			{c3, c4, c5},
			{c6, c7, c8}
		};
		QJsonObject j0({
						   {"intProperty", 0},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("0")},
						   {"doubleProperty", 0.0},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j1({
						   {"intProperty", 1},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("1")},
						   {"doubleProperty", 1.1},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j2({
						   {"intProperty", 2},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("2")},
						   {"doubleProperty", 2.2},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j3({
						   {"intProperty", 3},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("3")},
						   {"doubleProperty", 3.3},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j4({
						   {"intProperty", 4},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("4")},
						   {"doubleProperty", 4.4},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j5({
						   {"intProperty", 5},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("5")},
						   {"doubleProperty", 5.5},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j6({
						   {"intProperty", 6},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("6")},
						   {"doubleProperty", 6.6},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j7({
						   {"intProperty", 7},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("7")},
						   {"doubleProperty", 7.7},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonObject j8({
						   {"intProperty", 8},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("8")},
						   {"doubleProperty", 8.8},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"childObject", QJsonValue::Null},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()}
					   });
		QJsonArray jjl = {
			QJsonArray({j0, j1, j2}),
			QJsonArray({j3, j4, j5}),
			QJsonArray({j6, j7, j8})
		};
		QTest::newRow("list<childlist>") << TestObject::createChild(nullptr, {}, ccl, this)
										 << QJsonObject({
															{"intProperty", 0},
															{"boolProperty", false},
															{"stringProperty", QString()},
															{"doubleProperty", 0},
															{"simpeList", QJsonArray()},
															{"leveledList", QJsonArray()},
															{"childObject", QJsonValue::Null},
															{"simpleChildren", QJsonArray()},
															{"leveledChildren", jjl}
														})
										 << true;
	}

	{
		auto gc = TestObject::createBasic(42, true, "baum", 4.2);
		auto c = TestObject::createChild(gc, {}, {});
		QTest::newRow("child<child>") << TestObject::createChild(c, {}, {}, this)
									  << QJsonObject({
														 {"intProperty", 0},
														 {"boolProperty", false},
														 {"stringProperty", QString()},
														 {"doubleProperty", 0},
														 {"simpeList", QJsonArray()},
														 {"leveledList", QJsonArray()},
														 {"childObject", QJsonObject({
															  {"intProperty", 0},
															  {"boolProperty", false},
															  {"stringProperty", QString()},
															  {"doubleProperty", 0},
															  {"simpeList", QJsonArray()},
															  {"leveledList", QJsonArray()},
															  {"childObject", QJsonObject({
																   {"intProperty", 42},
																   {"boolProperty", true},
																   {"stringProperty", QStringLiteral("baum")},
																   {"doubleProperty", 4.2},
																   {"simpeList", QJsonArray()},
																   {"leveledList", QJsonArray()},
																   {"childObject", QJsonValue::Null},
																   {"simpleChildren", QJsonArray()},
																   {"leveledChildren", QJsonArray()}
															   })},
															  {"simpleChildren", QJsonArray()},
															  {"leveledChildren", QJsonArray()}
														  })},
														 {"simpleChildren", QJsonArray()},
														 {"leveledChildren", QJsonArray()}
													 })
									  << true;
	}
}

void SerializerTest::testObjectSerialization()
{
	QFETCH(TestObject*, object);
	QFETCH(QJsonObject, result);
	QFETCH(bool, works);

	if(works)
		QCOMPARE(serializer->serializeObj(object), result);
	else
		QVERIFY_EXCEPTION_THROWN(serializer->serializeObj(object), SerializerException);

	object->deleteLater();
}

QTEST_MAIN(SerializerTest)

#include "tst_serializer.moc"
