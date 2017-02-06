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
