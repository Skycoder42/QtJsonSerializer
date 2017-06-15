#include "brokentestobject.h"
#include "testobject.h"

#include <QString>
#include <QtTest>
#include <QtJsonSerializer/qjsonserializer.h>

class ObjectSerializerTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testVariantConversions_data();
	void testVariantConversions();
	void testSerialization_data();
	void testSerialization();
	void testDeserialization_data();
	void testDeserialization();

	void testObjectNameSerialization();
	void testNullDeserialization();

	void testEnumSpecialSerialization_data();
	void testEnumSpecialSerialization();
	void testEnumSpecialDeserialization_data();
	void testEnumSpecialDeserialization();

	void testDeviceSerialization_data();
	void testDeviceSerialization();

private:
	QJsonSerializer *serializer;

	void generateValidTestData();
};

void ObjectSerializerTest::initTestCase()
{
#ifdef Q_OS_LINUX
	Q_ASSERT(qgetenv("LD_PRELOAD").contains("Qt5JsonSerializer"));
#endif
	QJsonSerializer::registerListConverters<QList<int>>();
	QJsonSerializer::registerListConverters<TestObject*>();
	QJsonSerializer::registerListConverters<QList<TestObject*>>();
	//register list comparators, needed for test only!
	QMetaType::registerComparators<QList<int>>();
	QMetaType::registerComparators<QList<QList<int>>>();
	QMetaType::registerComparators<QList<TestObject*>>();
	QMetaType::registerComparators<QList<QList<TestObject*>>>();
	serializer = new QJsonSerializer(this);

	qMetaTypeId<TestObject*>();
}

void ObjectSerializerTest::cleanupTestCase()
{
	serializer->deleteLater();
	serializer = nullptr;
}

void ObjectSerializerTest::testVariantConversions_data()
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

void ObjectSerializerTest::testVariantConversions()
{
	QFETCH(QVariant, data);
	QFETCH(int, targetType);

	auto origType = data.userType();
	auto convData = data;
	QVERIFY(convData.convert(targetType));
	QVERIFY(convData.convert(origType));
	QCOMPARE(convData, data);
}

void ObjectSerializerTest::testSerialization_data()
{
	QTest::addColumn<TestObject*>("object");
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<bool>("works");

	generateValidTestData();

	QTest::newRow("invalidType") << (TestObject*)new BrokenTestObject(this)
								 << QJsonObject()
								 << false;
}

void ObjectSerializerTest::testSerialization()
{
	QFETCH(TestObject*, object);
	QFETCH(QJsonObject, result);
	QFETCH(bool, works);

	try {
		if(works)
			QCOMPARE(serializer->serialize(object), result);
		else
			QVERIFY_EXCEPTION_THROWN(serializer->serialize(object), QJsonSerializerException);
	} catch(QException &e) {
		QFAIL(e.what());
	}

	object->deleteLater();
}

void ObjectSerializerTest::testDeserialization_data()
{
	QTest::addColumn<TestObject*>("result");
	QTest::addColumn<QJsonObject>("data");
	QTest::addColumn<bool>("works");

	generateValidTestData();

	QTest::newRow("invalidType") << (TestObject*)new BrokenTestObject(this)
								 << QJsonObject({
													{"intProperty", 0},
													{"boolProperty", false},
													{"stringProperty", QString()},
													{"doubleProperty", 0},
													{"normalEnumProperty", TestObject::Normal0},
													{"enumFlagsProperty", 0},
													{"simpleList", QJsonArray()},
													{"leveledList", QJsonArray()},
													{"childObject", QJsonValue::Null},
													{"simpleChildren", QJsonArray()},
													{"leveledChildren", QJsonArray()},
													{"broken", QJsonValue::Null}
												})
								 << false;

	QTest::newRow("uncreatableType") << (TestObject*)new BrokenTestObject2(this)
									 << QJsonObject()
									 << false;
}

void ObjectSerializerTest::testDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(TestObject*, result);
	QFETCH(bool, works);

	try {
		if(works) {
			auto obj = serializer->deserialize<TestObject*>(data, this);
			QVERIFY(obj);
			QVERIFY(result->equals(obj));
			obj->deleteLater();
		} else {
			auto broken2 = qobject_cast<BrokenTestObject2*>(result);
			auto broken = qobject_cast<BrokenTestObject*>(result);
			if(broken2)
				QVERIFY_EXCEPTION_THROWN(serializer->deserialize<BrokenTestObject2*>(data, this), QJsonSerializerException);
			else if(broken)
				QVERIFY_EXCEPTION_THROWN(serializer->deserialize<BrokenTestObject*>(data, this), QJsonSerializerException);
			else
				QFAIL("Expected BrokenTestObject or BrokenTestObject2!");
		}
	} catch(QException &e) {
		QFAIL(e.what());
	}

	result->deleteLater();
}

void ObjectSerializerTest::testObjectNameSerialization()
{
	auto testObj = new TestObject(this);
	testObj->setObjectName("test");
	auto testJson = QJsonObject({
									{"intProperty", 0},
									{"boolProperty", false},
									{"stringProperty", QString()},
									{"doubleProperty", 0},
									{"normalEnumProperty", TestObject::Normal0},
									{"enumFlagsProperty", 0},
									{"simpleList", QJsonArray()},
									{"leveledList", QJsonArray()},
									{"childObject", QJsonValue::Null},
									{"simpleChildren", QJsonArray()},
									{"leveledChildren", QJsonArray()}
								});

	try {
		serializer->setKeepObjectName(false);
		auto json = serializer->serialize(testObj);
		QCOMPARE(json, testJson);

		serializer->setKeepObjectName(true);
		testJson["objectName"] = "test";
		json = serializer->serialize(testObj);
		QCOMPARE(json, testJson);
	} catch(QException &e) {
		QFAIL(e.what());
	}

	testObj->deleteLater();
	serializer->setKeepObjectName(false);
}

void ObjectSerializerTest::testNullDeserialization()
{
	auto testObj = new TestObject(this);
	auto testJson = QJsonObject({
									{"intProperty", QJsonValue::Null},
									{"boolProperty", QJsonValue::Null},
									{"stringProperty", QJsonValue::Null},
									{"doubleProperty", QJsonValue::Null},
									{"normalEnumProperty", QJsonValue::Null},
									{"enumFlagsProperty", QJsonValue::Null},
									{"simpleList", QJsonValue::Null},
									{"leveledList", QJsonValue::Null},
									{"childObject", QJsonValue::Null},
									{"simpleChildren", QJsonValue::Null},
									{"leveledChildren", QJsonValue::Null}
								});

	try {
		serializer->setAllowDefaultNull(false);
		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestObject*>(testJson, this), QJsonSerializerException);

		serializer->setAllowDefaultNull(true);
		auto obj = serializer->deserialize<TestObject*>(testJson, this);
		QVERIFY(obj);
		QVERIFY(testObj->equals(obj));
		obj->deleteLater();
	} catch(QException &e) {
		QFAIL(e.what());
	}

	testObj->deleteLater();
	serializer->setAllowDefaultNull(false);
}

void ObjectSerializerTest::testEnumSpecialSerialization_data()
{
	QTest::addColumn<TestObject*>("object");
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<bool>("asString");

	QTest::newRow("normal") << TestObject::createEnum(TestObject::Normal1, 0, this)
							<< QJsonObject({
											   {"intProperty", 0},
											   {"boolProperty", false},
											   {"stringProperty", QString()},
											   {"doubleProperty", 0},
											   {"normalEnumProperty", TestObject::Normal1},
											   {"enumFlagsProperty", 0},
											   {"simpleList", QJsonArray()},
											   {"leveledList", QJsonArray()},
											   {"childObject", QJsonValue::Null},
											   {"simpleChildren", QJsonArray()},
											   {"leveledChildren", QJsonArray()}
										   })
							<< false;

	QTest::newRow("stringEnum") << TestObject::createEnum(TestObject::Normal1, 0, this)
								<< QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", "Normal1"},
												   {"enumFlagsProperty", QString()},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"childObject", QJsonValue::Null},
												   {"simpleChildren", QJsonArray()},
												   {"leveledChildren", QJsonArray()}
											   })
								<< true;

	QTest::newRow("stringFlags") << TestObject::createEnum(TestObject::Normal0, TestObject::FlagX, this)
								<< QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", "Normal0"},
												   {"enumFlagsProperty", "FlagX"},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"childObject", QJsonValue::Null},
												   {"simpleChildren", QJsonArray()},
												   {"leveledChildren", QJsonArray()}
											   })
								<< true;

	QTest::newRow("stringMultiFlags") << TestObject::createEnum(TestObject::Normal2, TestObject::Flag1 | TestObject::Flag3, this)
									  << QJsonObject({
														 {"intProperty", 0},
														 {"boolProperty", false},
														 {"stringProperty", QString()},
														 {"doubleProperty", 0},
														 {"normalEnumProperty", "Normal2"},
														 {"enumFlagsProperty", "Flag1|Flag3"},
														 {"simpleList", QJsonArray()},
														 {"leveledList", QJsonArray()},
														 {"childObject", QJsonValue::Null},
														 {"simpleChildren", QJsonArray()},
														 {"leveledChildren", QJsonArray()}
													 })
									  << true;
}

void ObjectSerializerTest::testEnumSpecialSerialization()
{
	QFETCH(TestObject*, object);
	QFETCH(QJsonObject, result);
	QFETCH(bool, asString);

	try {
		serializer->setEnumAsString(asString);
		QCOMPARE(serializer->serialize(object), result);
	} catch(QException &e) {
		QFAIL(e.what());
	}

	object->deleteLater();
}

void ObjectSerializerTest::testEnumSpecialDeserialization_data()
{
	QTest::addColumn<TestObject*>("result");
	QTest::addColumn<QJsonObject>("data");

	QTest::newRow("normal") << TestObject::createEnum(TestObject::Normal1, 0, this)
							<< QJsonObject({
											   {"intProperty", 0},
											   {"boolProperty", false},
											   {"stringProperty", QString()},
											   {"doubleProperty", 0},
											   {"normalEnumProperty", TestObject::Normal1},
											   {"enumFlagsProperty", 0},
											   {"simpleList", QJsonArray()},
											   {"leveledList", QJsonArray()},
											   {"childObject", QJsonValue::Null},
											   {"simpleChildren", QJsonArray()},
											   {"leveledChildren", QJsonArray()}
										   });

	QTest::newRow("stringEnum") << TestObject::createEnum(TestObject::Normal1, 0, this)
								<< QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", "Normal1"},
												   {"enumFlagsProperty", 0},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"childObject", QJsonValue::Null},
												   {"simpleChildren", QJsonArray()},
												   {"leveledChildren", QJsonArray()}
											   });

	QTest::newRow("stringFlags") << TestObject::createEnum(TestObject::Normal0, TestObject::FlagX, this)
								<< QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", "Normal0"},
												   {"enumFlagsProperty", "FlagX"},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"childObject", QJsonValue::Null},
												   {"simpleChildren", QJsonArray()},
												   {"leveledChildren", QJsonArray()}
											   });

	QTest::newRow("stringMultiFlags") << TestObject::createEnum(TestObject::Normal2, TestObject::Flag1 | TestObject::Flag3, this)
									  << QJsonObject({
														 {"intProperty", 0},
														 {"boolProperty", false},
														 {"stringProperty", QString()},
														 {"doubleProperty", 0},
														 {"normalEnumProperty", "Normal2"},
														 {"enumFlagsProperty", "Flag1|Flag3"},
														 {"simpleList", QJsonArray()},
														 {"leveledList", QJsonArray()},
														 {"childObject", QJsonValue::Null},
														 {"simpleChildren", QJsonArray()},
														 {"leveledChildren", QJsonArray()}
													 });
}

void ObjectSerializerTest::testEnumSpecialDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(TestObject*, result);

	try {
		auto obj = serializer->deserialize<TestObject*>(data, this);
		QVERIFY(obj);
		QVERIFY(result->equals(obj));
		obj->deleteLater();
	} catch(QException &e) {
		QFAIL(e.what());
	}

	result->deleteLater();
}

void ObjectSerializerTest::testDeviceSerialization_data()
{
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QByteArray>("fakeDevice");
	QTest::addColumn<bool>("works");

	QTest::newRow("object") << QVariant::fromValue(TestObject::createBasic(42, true, "baum", 4.2, this))
							<< QByteArray()
							<< true;

	QTest::newRow("list") << QVariant::fromValue(QList<TestObject*>({TestObject::createBasic(42, true, "baum", 4.2, this), new TestObject(this)}))
						  << QByteArray()
						  << true;

	QTest::newRow("data") << QVariant(42)
						  << QByteArray()
						  << false;

	QTest::newRow("object") << QVariant::fromValue(new TestObject(this))
							<< QByteArray("invalid stuff")
							<< true;

}

void ObjectSerializerTest::testDeviceSerialization()
{
	QFETCH(QVariant, data);
	QFETCH(QByteArray, fakeDevice);
	QFETCH(bool, works);

	try {
		QTemporaryFile tFile;
		QVERIFY(tFile.open());
		if(works)
			serializer->serializeTo(&tFile, data);
		else {
			QVERIFY_EXCEPTION_THROWN(serializer->serializeTo(&tFile, data), QJsonSerializerException);
			return;
		}

		tFile.close();
		if(!fakeDevice.isEmpty()){
			QBuffer buffer(&fakeDevice);
			buffer.open(QIODevice::ReadOnly);
			QVERIFY_EXCEPTION_THROWN(serializer->deserializeFrom(&buffer, data.userType(), this), QJsonSerializerException);
		} else {
			QVERIFY(tFile.open());
			auto res = serializer->deserializeFrom(&tFile, data.userType(), this);
			if(data.userType() == qMetaTypeId<TestObject*>())
				QVERIFY(res.value<TestObject*>()->equals(data.value<TestObject*>()));
			else if(data.userType() == qMetaTypeId<QList<TestObject*>>())
				QCOMPARE(res.value<QList<TestObject*>>().size(), data.value<QList<TestObject*>>().size());
			else
				QCOMPARE(res, data);
		}

		tFile.close();
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void ObjectSerializerTest::generateValidTestData()
{
	QTest::newRow("default") << new TestObject(this)
							 << QJsonObject({
												{"intProperty", 0},
												{"boolProperty", false},
												{"stringProperty", QString()},
												{"doubleProperty", 0},
												{"normalEnumProperty", TestObject::Normal0},//converted to int
												{"enumFlagsProperty", 0},
												{"simpleList", QJsonArray()},
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
											  {"normalEnumProperty", TestObject::Normal0},
											  {"enumFlagsProperty", 0},
											  {"simpleList", QJsonArray()},
											  {"leveledList", QJsonArray()},
											  {"childObject", QJsonValue::Null},
											  {"simpleChildren", QJsonArray()},
											  {"leveledChildren", QJsonArray()}
										  })
						   << true;

	QTest::newRow("enum") << TestObject::createEnum(TestObject::Normal2, 0, this)
						  << QJsonObject({
											 {"intProperty", 0},
											 {"boolProperty", false},
											 {"stringProperty", QString()},
											 {"doubleProperty", 0},
											 {"normalEnumProperty", TestObject::Normal2},
											 {"enumFlagsProperty", 0},
											 {"simpleList", QJsonArray()},
											 {"leveledList", QJsonArray()},
											 {"childObject", QJsonValue::Null},
											 {"simpleChildren", QJsonArray()},
											 {"leveledChildren", QJsonArray()}
										 })
						  << true;
	QTest::newRow("flags") << TestObject::createEnum(TestObject::Normal0, TestObject::Flag1 | TestObject::Flag2, this)
						   << QJsonObject({
											  {"intProperty", 0},
											  {"boolProperty", false},
											  {"stringProperty", QString()},
											  {"doubleProperty", 0},
											  {"normalEnumProperty", TestObject::Normal0},
											  {"enumFlagsProperty", TestObject::FlagX},
											  {"simpleList", QJsonArray()},
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
											 {"normalEnumProperty", TestObject::Normal0},
											 {"enumFlagsProperty", 0},
											 {"simpleList", QJsonArray({3, 7, 13})},
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
													   {"normalEnumProperty", TestObject::Normal0},
													   {"enumFlagsProperty", 0},
													   {"simpleList", QJsonArray({3, 7, 13})},
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
											  {"normalEnumProperty", TestObject::Normal0},
											  {"enumFlagsProperty", 0},
											  {"simpleList", QJsonArray()},
											  {"leveledList", QJsonArray()},
											  {"childObject", QJsonObject({
												   {"intProperty", 42},
												   {"boolProperty", true},
												   {"stringProperty", QStringLiteral("baum")},
												   {"doubleProperty", 4.2},
												   {"normalEnumProperty", TestObject::Normal0},
												   {"enumFlagsProperty", 0},
												   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
													  {"normalEnumProperty", TestObject::Normal0},
													  {"enumFlagsProperty", 0},
													  {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
						   {"normalEnumProperty", TestObject::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
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
															{"normalEnumProperty", TestObject::Normal0},
															{"enumFlagsProperty", 0},
															{"simpleList", QJsonArray()},
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
														 {"normalEnumProperty", TestObject::Normal0},
														 {"enumFlagsProperty", 0},
														 {"simpleList", QJsonArray()},
														 {"leveledList", QJsonArray()},
														 {"childObject", QJsonObject({
															  {"intProperty", 0},
															  {"boolProperty", false},
															  {"stringProperty", QString()},
															  {"doubleProperty", 0},
															  {"normalEnumProperty", TestObject::Normal0},
															  {"enumFlagsProperty", 0},
															  {"simpleList", QJsonArray()},
															  {"leveledList", QJsonArray()},
															  {"childObject", QJsonObject({
																   {"intProperty", 42},
																   {"boolProperty", true},
																   {"stringProperty", QStringLiteral("baum")},
																   {"doubleProperty", 4.2},
																   {"normalEnumProperty", TestObject::Normal0},
																   {"enumFlagsProperty", 0},
																   {"simpleList", QJsonArray()},
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

static void compile_test()
{
	QJsonSerializer s;
	QVariant v;
	TestObject *t;
	QList<TestObject*> l;
	QIODevice *d = nullptr;
	QByteArray b;
	QJsonValue jv;
	QJsonObject jo;
	QJsonArray ja;
	QObject *p = nullptr;

	jv = s.serialize(v);
	jo = s.serialize(t);
	ja = s.serialize(l);

	s.serializeTo(d, v);
	s.serializeTo(d, t);
	s.serializeTo(d, l);

	b = s.serializeTo(v);
	b = s.serializeTo(t);
	b = s.serializeTo(l);

	v = s.deserialize(jv, qMetaTypeId<TestObject*>());
	v = s.deserialize(jv, qMetaTypeId<TestObject*>(), p);
	t = s.deserialize<TestObject*>(jo);
	t = s.deserialize<TestObject*>(jo, p);
	l = s.deserialize<QList<TestObject*>>(ja);
	l = s.deserialize<QList<TestObject*>>(ja, p);

	v = s.deserializeFrom(d, qMetaTypeId<TestObject*>());
	v = s.deserializeFrom(d, qMetaTypeId<TestObject*>(), p);
	t = s.deserializeFrom<TestObject*>(d);
	t = s.deserializeFrom<TestObject*>(d, p);
	l = s.deserializeFrom<QList<TestObject*>>(d);
	l = s.deserializeFrom<QList<TestObject*>>(d, p);

	v = s.deserializeFrom(b, qMetaTypeId<TestObject*>());
	v = s.deserializeFrom(b, qMetaTypeId<TestObject*>(), p);
	t = s.deserializeFrom<TestObject*>(b);
	t = s.deserializeFrom<TestObject*>(b, p);
	l = s.deserializeFrom<QList<TestObject*>>(b);
	l = s.deserializeFrom<QList<TestObject*>>(b, p);
}

QTEST_MAIN(ObjectSerializerTest)

#include "tst_objectserializer.moc"
