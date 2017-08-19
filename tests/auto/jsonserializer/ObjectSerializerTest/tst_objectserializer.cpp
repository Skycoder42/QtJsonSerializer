#include "brokentestobject.h"
#include "testobject.h"
#include "testpolyobject.h"

#include <QString>
#include <QtTest>
#include <QtJsonSerializer/qjsonserializer.h>

template <typename T>
bool operator <(const QMap<QString, T> &m1, const QMap<QString, T> &m2)
{
	return m1.keys() < m2.keys() &&
			m1.values() < m2.values();
}

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
	void testDeserializationValidation_data();
	void testDeserializationValidation();

	void testEnumSpecialSerialization_data();
	void testEnumSpecialSerialization();
	void testEnumSpecialDeserialization_data();
	void testEnumSpecialDeserialization();

	void testPolySerialization_data();
	void testPolySerialization();
	void testPolyDeserialization_data();
	void testPolyDeserialization();

	void testDeviceSerialization_data();
	void testDeviceSerialization();

	void testExceptionTrace();

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
	QJsonSerializer::registerMapConverters<QMap<QString, int>>();

	QJsonSerializer::registerAllConverters<ChildObject*>();
	QJsonSerializer::registerListConverters<QList<ChildObject*>>();
	QJsonSerializer::registerMapConverters<QMap<QString, ChildObject*>>();

	QJsonSerializer::registerAllConverters<TestObject*>();
	QJsonSerializer::registerPointerConverters<ChildObject>();

	QJsonSerializer::registerPairConverters<int, QString>();
	QJsonSerializer::registerPairConverters<bool, bool>();
	QJsonSerializer::registerPairConverters<ChildObject*, QList<int>>();
	QJsonSerializer::registerListConverters<QPair<bool, bool>>();

	QJsonSerializer::registerAllConverters<TestPolyObject*>();

	//register list comparators, needed for test only!
	QMetaType::registerComparators<QList<int>>();
	QMetaType::registerComparators<QList<QList<int>>>();
	QMetaType::registerComparators<QMap<QString, int>>();
	QMetaType::registerComparators<QMap<QString, QMap<QString, int>>>();

	QMetaType::registerComparators<ChildObject*>();
	QMetaType::registerComparators<QList<ChildObject*>>();
	QMetaType::registerComparators<QList<QList<ChildObject*>>>();
	QMetaType::registerComparators<QMap<QString, ChildObject*>>();
	QMetaType::registerComparators<QMap<QString, QMap<QString, ChildObject*>>>();

	QMetaType::registerComparators<TestObject*>();
	QMetaType::registerComparators<QList<TestObject*>>();

	QMetaType::registerComparators<QPair<int, QString>>();
	QMetaType::registerComparators<QPair<ChildObject*, QList<int>>>();
	QMetaType::registerComparators<QList<QPair<bool, bool>>>();

	serializer = new QJsonSerializer(this);
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
	QTest::newRow("QStringList") << QVariant::fromValue<QStringList>({"a", "b", "c"})
								 << (int)QVariant::List;

	QTest::newRow("QMap<QString, int>") << QVariant::fromValue<QMap<QString, int>>({
																					   {"baum", 42},
																					   {"devil", 666},
																					   {"fun", 0}
																				   })
										<< (int)QVariant::Map;
	QMap<QString, int> m1 = {{"v0", 0}, {"v1", 1}, {"v2", 2}};
	QMap<QString, int> m2 = {{"v3", 3}, {"v4", 4}, {"v5", 5}};
	QMap<QString, int> m3 = {{"v6", 6}, {"v7", 7}, {"v8", 8}};
	QTest::newRow("QMap<QString, QMap<QString, int>>") << QVariant::fromValue<QMap<QString, QMap<QString, int>>>({
																													 {"m1", m1},
																													 {"m2", m2},
																													 {"m3", m3}
																												 })
									   << (int)QVariant::Map;

	QTest::newRow("TestObject*") << QVariant::fromValue<TestObject*>(new TestObject(this))
								 << (int)QMetaType::QObjectStar;
	QTest::newRow("ChildObject*") << QVariant::fromValue<ChildObject*>(new ChildObject(this))
								 << (int)QMetaType::QObjectStar;
	QTest::newRow("QList<ChildObject*>") << QVariant::fromValue<QList<ChildObject*>>({
																						 new ChildObject(this),
																						 new ChildObject(this),
																						 new ChildObject(this)
																					 })
										 << (int)QVariant::List;
	QList<ChildObject*> o1 = {
		new ChildObject(this),
		new ChildObject(this),
		new ChildObject(this)
	};
	QList<ChildObject*> o2 = {
		new ChildObject(this),
		new ChildObject(this),
		new ChildObject(this)
	};
	QList<ChildObject*> o3 = {
		new ChildObject(this),
		new ChildObject(this),
		new ChildObject(this)
	};
	QTest::newRow("QList<QList<ChildObject*>>") << QVariant::fromValue<QList<QList<ChildObject*>>>({o1, o2, o3})
											   << (int)QVariant::List;

	QTest::newRow("QMap<QString, ChildObject*>") << QVariant::fromValue<QMap<QString, ChildObject*>>({
																									   {"baum", new ChildObject(this)},
																									   {"devil", new ChildObject(this)},
																									   {"fun", new ChildObject(this)}
																								   })
												<< (int)QVariant::Map;
	QMap<QString, ChildObject*> r1 = {
		{"v0", new ChildObject(this)},
		{"v1", new ChildObject(this)},
		{"v2", new ChildObject(this)}};
	QMap<QString, ChildObject*> r2 = {
		{"v3", new ChildObject(this)},
		{"v4", new ChildObject(this)},
		{"v5", new ChildObject(this)}};
	QMap<QString, ChildObject*> r3 = {
		{"v6", new ChildObject(this)},
		{"v7", new ChildObject(this)},
		{"v8", new ChildObject(this)}};
	QTest::newRow("QMap<QString, QMap<QString, ChildObject*>>") << QVariant::fromValue<QMap<QString, QMap<QString, ChildObject*>>>({
																																	 {"m1", r1},
																																	 {"m2", r2},
																																	 {"m3", r3}
																																 })
															   << (int)QVariant::Map;

	QTest::newRow("QSharedPointer<ChildObject>") << QVariant::fromValue(QSharedPointer<ChildObject>(new ChildObject(nullptr)))
												 << qMetaTypeId<QSharedPointer<QObject>>();
	QTest::newRow("QPointer<ChildObject>") << QVariant::fromValue<QPointer<ChildObject>>(new ChildObject(this))
										   << qMetaTypeId<QPointer<QObject>>();

	QTest::newRow("QPair<int, QString>") << QVariant::fromValue<QPair<int, QString>>({42, "baum"})
										 << qMetaTypeId<QPair<QVariant, QVariant>>();

	QTest::newRow("QPair<ChildObject*, QList<int>>") << QVariant::fromValue<QPair<ChildObject*, QList<int>>>({nullptr, {1, 2, 3}})
													<< qMetaTypeId<QPair<QVariant, QVariant>>();

	QTest::newRow("QList<QPair<bool, bool>>") << QVariant::fromValue<QList<QPair<bool, bool>>>({{false, true}, {true, false}})
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
			QVERIFY_EXCEPTION_THROWN(serializer->serialize((BrokenTestObject*)object), QJsonSerializerException);
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
								 << TestObject::createJson({
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
	auto testJson = TestObject::createJson();

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
									{"simpleMap", QJsonValue::Null},
									{"leveledMap", QJsonValue::Null},
									{"childObject", QJsonValue::Null},
									{"simpleChildren", QJsonValue::Null},
									{"leveledChildren", QJsonValue::Null},
									{"simpleRelatives", QJsonValue::Null},
									{"leveledRelatives", QJsonValue::Null}
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

void ObjectSerializerTest::testDeserializationValidation_data()
{
	QTest::addColumn<QJsonObject>("data");
	QTest::addColumn<QJsonSerializer::ValidationFlags>("flags");
	QTest::addColumn<bool>("success");

	QTest::newRow("standard") << TestObject::createJson({
															{"garbage", QJsonValue::Null} //extra property
														}, "leveledMap")
							  << (QJsonSerializer::ValidationFlags)QJsonSerializer::StandardValidation
							  << true;

	QTest::newRow("validateExtra") << TestObject::createJson({
																 {"garbage", QJsonValue::Null} //extra property
															 })
								   << (QJsonSerializer::ValidationFlags)QJsonSerializer::NoExtraProperties
								   << false;

	QTest::newRow("validateMissing") << TestObject::createJson({}, "leveledMap")
									 << (QJsonSerializer::ValidationFlags)QJsonSerializer::AllProperties
									 << false;

	QTest::newRow("validateAll") << TestObject::createJson({
															   {"garbage", QJsonValue::Null} //extra property
														   }, "leveledMap")
								 << (QJsonSerializer::ValidationFlags)QJsonSerializer::FullValidation
								 << false;
}

void ObjectSerializerTest::testDeserializationValidation()
{
	QFETCH(QJsonObject, data);
	QFETCH(QJsonSerializer::ValidationFlags, flags);
	QFETCH(bool, success);

	serializer->setValidationFlags(flags);

	auto t1 = new TestObject(this);

	try {
		if(success) {
			auto deser = serializer->deserialize<TestObject*>(data, this);
			QVERIFY(t1->equals(deser));
			deser->deleteLater();
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestObject*>(data, this), QJsonDeserializationException);
	} catch (QException &e) {
		QFAIL(e.what());
	}

	serializer->setValidationFlags(QJsonSerializer::StandardValidation);
	t1->deleteLater();
}

void ObjectSerializerTest::testEnumSpecialSerialization_data()
{
	QTest::addColumn<TestObject*>("object");
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<bool>("asString");

	QTest::newRow("normal") << TestObject::createEnum(TestObject::Normal1, 0, this)
							<< TestObject::createJson({
														  {"normalEnumProperty", TestObject::Normal1},
														  {"enumFlagsProperty", 0}
													  })
							<< false;

	QTest::newRow("stringEnum") << TestObject::createEnum(TestObject::Normal1, 0, this)
								<< TestObject::createJson({
															  {"normalEnumProperty", "Normal1"},
															  {"enumFlagsProperty", QString()},
														  })
								<< true;

	QTest::newRow("stringFlags") << TestObject::createEnum(TestObject::Normal0, TestObject::FlagX, this)
								 << TestObject::createJson({
															   {"normalEnumProperty", "Normal0"},
															   {"enumFlagsProperty", "FlagX"}
														   })
								 << true;

	QTest::newRow("stringMultiFlags") << TestObject::createEnum(TestObject::Normal0, TestObject::Flag1 | TestObject::Flag3, this)
									  << TestObject::createJson({
																	{"normalEnumProperty", "Normal0"},
																	{"enumFlagsProperty", "Flag1|Flag3"}
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

	serializer->setEnumAsString(false);
	object->deleteLater();
}

void ObjectSerializerTest::testEnumSpecialDeserialization_data()
{
	QTest::addColumn<TestObject*>("result");
	QTest::addColumn<QJsonObject>("data");
	QTest::addColumn<bool>("success");

	QTest::newRow("normal") << TestObject::createEnum(TestObject::Normal1, 0, this)
							<< TestObject::createJson({
														  {"normalEnumProperty", TestObject::Normal1},
														  {"enumFlagsProperty", 0}
													  })
							<< true;

	QTest::newRow("stringEnum") << TestObject::createEnum(TestObject::Normal1, 0, this)
								<< TestObject::createJson({
															  {"normalEnumProperty", "Normal1"}
														  })
								<< true;

	QTest::newRow("invalidEnum") << TestObject::createEnum(TestObject::Normal2, 0, this)
								 << TestObject::createJson({
															   {"normalEnumProperty", "baum"}
														   })
								 << false;

	QTest::newRow("stringFlags") << TestObject::createEnum(TestObject::Normal0, TestObject::FlagX, this)
								 << TestObject::createJson({
															   {"enumFlagsProperty", "FlagX"}
														   })
								 << true;

	QTest::newRow("stringMultiFlags") << TestObject::createEnum(TestObject::Normal0, TestObject::Flag1 | TestObject::Flag3, this)
									  << TestObject::createJson({
																	{"enumFlagsProperty", "Flag1|Flag3"}
																})
									  << true;

	QTest::newRow("invalidFlags") << TestObject::createEnum(TestObject::Normal0, TestObject::FlagX, this)
								  << TestObject::createJson({
																{"enumFlagsProperty", "baum"}
															})
								  << false;
}

void ObjectSerializerTest::testEnumSpecialDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(TestObject*, result);
	QFETCH(bool, success);

	try {
		if(success) {
			auto obj = serializer->deserialize<TestObject*>(data, this);
			QVERIFY(obj);
			QVERIFY(result->equals(obj));
			obj->deleteLater();
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestObject*>(data, this), QJsonDeserializationException);
	} catch(QException &e) {
		QFAIL(e.what());
	}

	result->deleteLater();
}

void ObjectSerializerTest::testPolySerialization_data()
{
	QTest::addColumn<QObject*>("data");
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<QJsonSerializer::Polymorphing>("poly");

	QObject *testObj = new TestObject(this);
	testObj->setObjectName("test");
	QTest::newRow("default") << testObj
							 << QJsonObject({
												{"objectName", "test"}
											})
							 << QJsonSerializer::Enabled;

	testObj = new TestObject(this);
	testObj->setObjectName("test");
	QTest::newRow("forced") << testObj
							 << TestObject::createJson({
														   {"@class", "TestObject"},
														   {"objectName", "test"}
													   })
							 << QJsonSerializer::Forced;

	testObj = new TestObject(this);
	testObj->setObjectName("test");
	testObj->setProperty("__qt_json_serializer_polymorphic", true);
	QTest::newRow("property") << testObj
							  << TestObject::createJson({
															{"@class", "TestObject"},
															{"objectName", "test"}
														})
							  << QJsonSerializer::Enabled;

	testObj = new TestObject(this);
	testObj->setObjectName("test");
	testObj->setProperty("__qt_json_serializer_polymorphic", true);
	QTest::newRow("propertyDisabled") << testObj
									  << QJsonObject({
														 {"objectName", "test"}
													 })
									  << QJsonSerializer::Disabled;

	testObj = new TestObject(this);
	testObj->setObjectName("test");
	testObj->setProperty("__qt_json_serializer_polymorphic", false);
	QTest::newRow("propertyForced") << testObj
									<< TestObject::createJson({
																  {"@class", "TestObject"},
																  {"objectName", "test"}
															  })
									<< QJsonSerializer::Forced;

	testObj = new TestPolyObject(10, this);
	testObj->setObjectName("test");
	QTest::newRow("classinfo") << testObj
							   << QJsonObject({
												  {"@class", "TestPolyObject"},
												  {"objectName", "test"},
												  {"data", 10}
											  })
							   << QJsonSerializer::Enabled;

	testObj = new TestPolyObject(10, this);
	testObj->setObjectName("test");
	QTest::newRow("classDisabled") << testObj
								   << QJsonObject({
													  {"objectName", "test"}
												  })
								   << QJsonSerializer::Disabled;

	testObj = new TestPolyObject(10, this);
	testObj->setObjectName("test");
	testObj->setProperty("__qt_json_serializer_polymorphic", false);
	QTest::newRow("classinfoProperty") << testObj
									   << QJsonObject({
														  {"objectName", "test"}
													  })
									   << QJsonSerializer::Enabled;
}

void ObjectSerializerTest::testPolySerialization()
{
	QFETCH(QObject*, data);
	QFETCH(QJsonObject, result);
	QFETCH(QJsonSerializer::Polymorphing, poly);

	serializer->setKeepObjectName(true);
	serializer->setPolymorphing(poly);

	try {
		QCOMPARE(serializer->serialize<QObject*>(data), result);
	} catch(QException &e) {
		QFAIL(e.what());
	}

	serializer->setPolymorphing(QJsonSerializer::Enabled);
	serializer->setKeepObjectName(false);

	data->deleteLater();
}

void ObjectSerializerTest::testPolyDeserialization_data()
{
	QTest::addColumn<QJsonObject>("data");
	QTest::addColumn<QObject*>("result");
	QTest::addColumn<QJsonSerializer::Polymorphing>("poly");
	QTest::addColumn<bool>("success");

	QObject *testObj = new QObject(this);
	testObj->setObjectName("test");
	QTest::newRow("default") << QJsonObject({
												{"objectName", "test"}
											})
							 << testObj
							 << QJsonSerializer::Enabled
							 << true;

	testObj = new QObject(this);
	QTest::newRow("forcedNotAvailable") << QJsonObject({
														   {"objectName", "test"}
													   })
										<< testObj
										<< QJsonSerializer::Forced
										<< false;

	testObj = new TestPolyObject(10, this);
	testObj->setObjectName("test");
	QTest::newRow("enabled") << QJsonObject({
												{"@class", "TestPolyObject"},
												{"objectName", "test"},
												{"data", 10}
											})
							 << testObj
							 << QJsonSerializer::Enabled
							 << true;

	testObj = new TestPolyObject(11, this);
	testObj->setObjectName("test");
	QTest::newRow("forced") << QJsonObject({
											   {"@class", "TestPolyObject"},
											   {"objectName", "test"},
											   {"data", 11}
										   })
							<< testObj
							<< QJsonSerializer::Forced
							<< true;

	testObj = new QObject(this);
	testObj->setObjectName("test");
	QTest::newRow("disabled") << QJsonObject({
												 {"@class", "TestPolyObject"},
												 {"objectName", "test"},
												 {"data", 11}
											 })
							  << testObj
							  << QJsonSerializer::Disabled
							  << true;

	testObj = new QObject(this);
	QTest::newRow("unavailableType") << QJsonObject({
														{"@class", "UnavailableObject"}
													})
									 << testObj
									 << QJsonSerializer::Enabled
									 << false;
}

void ObjectSerializerTest::testPolyDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(QObject*, result);
	QFETCH(QJsonSerializer::Polymorphing, poly);
	QFETCH(bool, success);

	serializer->setKeepObjectName(true);
	serializer->setPolymorphing(poly);

	try {
		if(success) {
			auto obj = serializer->deserialize<QObject*>(data, this);
			QVERIFY(obj);

			auto meta = result->metaObject();
			QCOMPARE(meta->className(), obj->metaObject()->className());
			auto i = QObject::staticMetaObject.indexOfProperty("objectName");
			for(; i < meta->propertyCount(); i++) {
				auto property = meta->property(i);
				QCOMPARE(property.read(obj), property.read(result));
			}

			obj->deleteLater();
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestObject*>(data, this), QJsonDeserializationException);
	} catch(QException &e) {
		QFAIL(e.what());
	}

	serializer->setPolymorphing(QJsonSerializer::Enabled);
	serializer->setKeepObjectName(false);

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

void ObjectSerializerTest::testExceptionTrace()
{
	try {
		auto o = serializer->deserialize<TestObject*>(TestObject::createJson({
																				 {"childObject", QJsonObject({
																					  {"data", "test"}
																				  })}
																			 }));
		o->deleteLater();
		QFAIL("No exception thrown");
	} catch (QJsonSerializerException &e) {
		auto trace = e.propertyTrace();
		QCOMPARE(trace.size(), 2);
		QCOMPARE(trace[0].first, QByteArray("childObject"));
		QCOMPARE(trace[0].second, QByteArray("ChildObject*"));
		QCOMPARE(trace[1].first, QByteArray("data"));
		QCOMPARE(trace[1].second, QByteArray("int"));
	}
}

void ObjectSerializerTest::generateValidTestData()
{
	QTest::newRow("default") << new TestObject(this)
							 << TestObject::createJson()
							 << true;

	QTest::newRow("basic") << TestObject::createBasic(42, true, "baum", 4.2, this)
						   << TestObject::createJson({
											  {"intProperty", 42},
											  {"boolProperty", true},
											  {"stringProperty", QStringLiteral("baum")},
											  {"doubleProperty", 4.2}
										  })
						   << true;

	QTest::newRow("enum") << TestObject::createEnum(TestObject::Normal2, 0, this)
						  << TestObject::createJson({
														{"normalEnumProperty", TestObject::Normal2}
													})
						  << true;
	QTest::newRow("flags") << TestObject::createEnum(TestObject::Normal0, TestObject::Flag1 | TestObject::Flag2, this)
						   << TestObject::createJson({
														 {"enumFlagsProperty", TestObject::FlagX}
													 })
						   << true;

	QTest::newRow("list") << TestObject::createList({3, 7, 13}, {}, this)
						  << TestObject::createJson({
														{"simpleList", QJsonArray({3, 7, 13})}
													})
						  << true;

	{
		QList<int> l1 = {0, 1, 2};
		QList<int> l2 = {3, 4, 5};
		QList<int> l3 = {6, 7, 8};
		QJsonArray j1 = {0, 1, 2};
		QJsonArray j2 = {3, 4, 5};
		QJsonArray j3 = {6, 7, 8};
		QTest::newRow("list<list>") << TestObject::createList({}, {l1, l2, l3}, this)
									<< TestObject::createJson({
																  {"leveledList", QJsonArray({j1, j2, j3})}
															  })
									<< true;
	}

	QTest::newRow("map") << TestObject::createMap({{"v3", 3}, {"v7", 7}, {"v13", 13}}, {}, this)
						 << TestObject::createJson({
													   {"simpleMap", QJsonObject({
															{"v3", 3},
															{"v7", 7},
															{"v13", 13}
														})}
												   })
						 << true;

	{
		QMap<QString, int> m1 = {{"v0", 0}, {"v1", 1}, {"v2", 2}};
		QMap<QString, int> m2 = {{"v3", 3}, {"v4", 4}, {"v5", 5}};
		QMap<QString, int> m3 = {{"v6", 6}, {"v7", 7}, {"v8", 8}};
		QJsonObject j1 = {{"v0", 0}, {"v1", 1}, {"v2", 2}};
		QJsonObject j2 = {{"v3", 3}, {"v4", 4}, {"v5", 5}};
		QJsonObject j3 = {{"v6", 6}, {"v7", 7}, {"v8", 8}};
		QTest::newRow("map<map>") << TestObject::createMap({}, {{"m1", m1}, {"m2", m2}, {"m3", m3}}, this)
								  << TestObject::createJson({
																{"leveledMap", QJsonObject({
																	 {"m1", j1},
																	 {"m2", j2},
																	 {"m3", j3}
																 })}
															})
								  << true;
	}

	QTest::newRow("pair") << TestObject::createPair({42, "baum"}, {}, {}, this)
						  << TestObject::createJson({
														{"pair", QJsonArray({42, "baum"})}
													})
						  << true;
	QTest::newRow("extraPair") << TestObject::createPair({}, {new ChildObject(42), {6, 6, 666}}, {}, this)
							   << TestObject::createJson({
															 {"extraPair", QJsonArray({ChildObject::createJson(42), QJsonArray({6, 6, 666})})}
														 })
							   << true;
	QTest::newRow("listPair") << TestObject::createPair({}, {}, {{true, false}, {false, true}}, this)
							  << TestObject::createJson({
															{"listPair", QJsonArray({QJsonArray({true, false}), QJsonArray({false, true})})}
														})
							  << true;

	QTest::newRow("child") << TestObject::createChild(new ChildObject(42), this)
						   << TestObject::createJson({
														 {"childObject", ChildObject::createJson(42)},
													 })
						   << true;

	QTest::newRow("sharedChild") << TestObject::createChild(new ChildObject(42), this, 1)
								 << TestObject::createJson({
															   {"sharedChildObject", ChildObject::createJson(42)},
														   })
								 << true;

	QTest::newRow("trackedChild") << TestObject::createChild(new ChildObject(42), this, 2)
								   << TestObject::createJson({
																 {"trackedChildObject", ChildObject::createJson(42)},
															 })
								   << true;

	{
		auto gc = new ChildObject(42);
		auto c = new ChildObject(666);
		c->child = gc;
		gc->setParent(c);

		QTest::newRow("child<child>") << TestObject::createChild(c, this)
									  << TestObject::createJson({
																	{"childObject", QJsonObject({
																		 {"data", 666},
																		 {"child", ChildObject::createJson(42)}
																	 })}
																})
									  << true;
	}

	QTest::newRow("childlist") << TestObject::createChildren({new ChildObject(3), new ChildObject(7), new ChildObject(13)}, {}, this)
							   << TestObject::createJson({
															 {"simpleChildren", QJsonArray({
																  ChildObject::createJson(3),
																  ChildObject::createJson(7),
																  ChildObject::createJson(13),
															  })},
														 })
							   << true;

	{
		QList<QList<ChildObject*>> ccl = {
			{new ChildObject(0), new ChildObject(1), new ChildObject(2)},
			{new ChildObject(3), new ChildObject(4), new ChildObject(5)},
			{new ChildObject(6), new ChildObject(7), new ChildObject(8)}
		};
		QJsonArray jjl = {
			QJsonArray({ChildObject::createJson(0), ChildObject::createJson(1), ChildObject::createJson(2)}),
			QJsonArray({ChildObject::createJson(3), ChildObject::createJson(4), ChildObject::createJson(5)}),
			QJsonArray({ChildObject::createJson(6), ChildObject::createJson(7), ChildObject::createJson(8)})
		};
		QTest::newRow("list<childlist>") << TestObject::createChildren({}, ccl, this)
										 << TestObject::createJson({
																	   {"leveledChildren", jjl}
																   })
										 << true;
	}

	QTest::newRow("childmap") << TestObject::createRelatives({
																 {"c1", new ChildObject(3)},
																 {"c2", new ChildObject(7)},
																 {"c3", new ChildObject(13)}
															 }, {}, this)
							  << TestObject::createJson({
															{"simpleRelatives", QJsonObject({
																 {"c1", ChildObject::createJson(3)},
																 {"c2", ChildObject::createJson(7)},
																 {"c3", ChildObject::createJson(13)},
															 })},
														})
							  << true;

	{
		QMap<QString, ChildObject*> cm1 = {
			{"c0", new ChildObject(0)},
			{"c1", new ChildObject(1)},
			{"c2", new ChildObject(2)}
		};
		QMap<QString, ChildObject*> cm2 = {
			{"c3", new ChildObject(3)},
			{"c4", new ChildObject(4)},
			{"c5", new ChildObject(5)}
		};
		QMap<QString, ChildObject*> cm3 = {
			{"c6", new ChildObject(6)},
			{"c7", new ChildObject(7)},
			{"c8", new ChildObject(8)}
		};
		QMap<QString, QMap<QString, ChildObject*>> ccm = {
			{"cm1", cm1},
			{"cm2", cm2},
			{"cm3", cm3}
		};

		QJsonObject j1({
			{"c0", ChildObject::createJson(0)},
			{"c1", ChildObject::createJson(1)},
			{"c2", ChildObject::createJson(2)}
		});
		QJsonObject j2({
			{"c3", ChildObject::createJson(3)},
			{"c4", ChildObject::createJson(4)},
			{"c5", ChildObject::createJson(5)},
		});
		QJsonObject j3({
			{"c6", ChildObject::createJson(6)},
			{"c7", ChildObject::createJson(7)},
			{"c8", ChildObject::createJson(8)},
		});
		QJsonObject jjm = {
			{"cm1", j1},
			{"cm2", j2},
			{"cm3", j3}
		};
		QTest::newRow("map<childmap>") << TestObject::createRelatives({}, ccm, this)
									   << TestObject::createJson({
																	 {"leveledRelatives", jjm}
																 })
									   << true;
	}

	QTest::newRow("jobject") << TestObject::createEmbedded(ChildObject::createJson(), QJsonArray(), QJsonValue(), this)
							 << TestObject::createJson({
														   {"object", ChildObject::createJson()}
													   })
							 << true;

	QTest::newRow("jarray") << TestObject::createEmbedded(QJsonObject(), QJsonArray({1, 2, 3}), QJsonValue(), this)
							<< TestObject::createJson({
														  {"array", QJsonArray({1, 2, 3})}
													  })
							<< true;

	QTest::newRow("jvalue") << TestObject::createEmbedded(QJsonObject(), QJsonArray(), 42, this)
							<< TestObject::createJson({
														  {"value", 42}
													  })
							<< true;
}

static void compile_test()
{
	QJsonSerializer s;
	QVariant v;
	TestObject *t;
	QList<TestObject*> l;
	QMap<QString, TestObject*> m;
	QIODevice *d = nullptr;
	QByteArray b;
	QJsonValue jv;
	QJsonObject jo;
	QJsonArray ja;
	QObject *p = nullptr;

	jv = s.serialize(v);
	jo = s.serialize(t);
	ja = s.serialize(l);
	jo = s.serialize(m);

	s.serializeTo(d, v);
	s.serializeTo(d, t);
	s.serializeTo(d, l);
	s.serializeTo(d, m);

	b = s.serializeTo(v);
	b = s.serializeTo(t);
	b = s.serializeTo(l);
	b = s.serializeTo(m);

	v = s.deserialize(jv, qMetaTypeId<TestObject*>());
	v = s.deserialize(jv, qMetaTypeId<TestObject*>(), p);
	t = s.deserialize<TestObject*>(jo);
	t = s.deserialize<TestObject*>(jo, p);
	l = s.deserialize<QList<TestObject*>>(ja);
	l = s.deserialize<QList<TestObject*>>(ja, p);
	m = s.deserialize<QMap<QString, TestObject*>>(jo);
	m = s.deserialize<QMap<QString, TestObject*>>(jo, p);

	v = s.deserializeFrom(d, qMetaTypeId<TestObject*>());
	v = s.deserializeFrom(d, qMetaTypeId<TestObject*>(), p);
	t = s.deserializeFrom<TestObject*>(d);
	t = s.deserializeFrom<TestObject*>(d, p);
	l = s.deserializeFrom<QList<TestObject*>>(d);
	l = s.deserializeFrom<QList<TestObject*>>(d, p);
	m = s.deserializeFrom<QMap<QString, TestObject*>>(d);
	m = s.deserializeFrom<QMap<QString, TestObject*>>(d, p);

	v = s.deserializeFrom(b, qMetaTypeId<TestObject*>());
	v = s.deserializeFrom(b, qMetaTypeId<TestObject*>(), p);
	t = s.deserializeFrom<TestObject*>(b);
	t = s.deserializeFrom<TestObject*>(b, p);
	l = s.deserializeFrom<QList<TestObject*>>(b);
	l = s.deserializeFrom<QList<TestObject*>>(b, p);
	m = s.deserializeFrom<QMap<QString, TestObject*>>(b);
	m = s.deserializeFrom<QMap<QString, TestObject*>>(b, p);
}

QTEST_MAIN(ObjectSerializerTest)

#include "tst_objectserializer.moc"
