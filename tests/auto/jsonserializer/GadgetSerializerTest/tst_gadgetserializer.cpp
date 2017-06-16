#include "brokentestgadget.h"
#include "testgadget.h"

#include <QtJsonSerializer/qjsonserializer.h>
#include <QString>
#include <QtTest>

template <typename T>
bool operator <(const QMap<QString, T> &m1, const QMap<QString, T> &m2)
{
	return m1.keys() < m2.keys() &&
			m1.values() < m2.values();
}

class GadgetSerializerTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testVariantConversions_data();
	void testVariantConversions();
	void testSerialization_data();
	void testSerialization();
//	void testInvalidSerialization();
	void testDeserialization_data();
	void testDeserialization();
//	void testInvalidDeserialization();

//	void testNullChild();
//	void testNullDeserialization();
//	void testDeserializationValidation_data();
//	void testDeserializationValidation();

//	void testEnumSpecialSerialization_data();
//	void testEnumSpecialSerialization();
//	void testEnumSpecialDeserialization_data();
//	void testEnumSpecialDeserialization();

//	void testDeviceSerialization_data();
//	void testDeviceSerialization();

private:
	QJsonSerializer *serializer;

	void generateValidTestData();
};

void GadgetSerializerTest::initTestCase()
{
#ifdef Q_OS_LINUX
	Q_ASSERT(qgetenv("LD_PRELOAD").contains("Qt5JsonSerializer"));
#endif
	QJsonSerializer::registerListConverters<QList<int>>();
	QJsonSerializer::registerMapConverters<QMap<QString, int>>();

	QJsonSerializer::registerAllConverters<ChildGadget>();
	QJsonSerializer::registerListConverters<QList<ChildGadget>>();
	QJsonSerializer::registerMapConverters<QMap<QString, ChildGadget>>();

	//register list comparators, needed for test only!
	QMetaType::registerComparators<ChildGadget>();
	QMetaType::registerComparators<QList<int>>();
	QMetaType::registerComparators<QList<QList<int>>>();
	QMetaType::registerComparators<QMap<QString, int>>();
	QMetaType::registerComparators<QMap<QString, QMap<QString, int>>>();
	QMetaType::registerComparators<QList<ChildGadget>>();
	QMetaType::registerComparators<QList<QList<ChildGadget>>>();
	QMetaType::registerComparators<QMap<QString, ChildGadget>>();
	QMetaType::registerComparators<QMap<QString, QMap<QString, ChildGadget>>>();
	serializer = new QJsonSerializer(this);
}

void GadgetSerializerTest::cleanupTestCase()
{
	serializer->deleteLater();
	serializer = nullptr;
}

void GadgetSerializerTest::testVariantConversions_data()
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

	QTest::newRow("QList<ChildGadget>") << QVariant::fromValue<QList<ChildGadget>>({
																					  ChildGadget(),
																					  ChildGadget(),
																					  ChildGadget()
																				  })
										<< (int)QVariant::List;
	QList<ChildGadget> o1 = {
		ChildGadget(),
		ChildGadget(),
		ChildGadget()
	};
	QList<ChildGadget> o2 = {
		ChildGadget(),
		ChildGadget(),
		ChildGadget()
	};
	QList<ChildGadget> o3 = {
		ChildGadget(),
		ChildGadget(),
		ChildGadget()
	};
	QTest::newRow("QList<QList<ChildGadget>>") << QVariant::fromValue<QList<QList<ChildGadget>>>({o1, o2, o3})
											   << (int)QVariant::List;

	QTest::newRow("QMap<QString, ChildGadget>") << QVariant::fromValue<QMap<QString, ChildGadget>>({
																									   {"baum", ChildGadget()},
																									   {"devil", ChildGadget()},
																									   {"fun", ChildGadget()}
																								   })
												<< (int)QVariant::Map;
	QMap<QString, ChildGadget> r1 = {
		{"v0", ChildGadget()},
		{"v1", ChildGadget()},
		{"v2", ChildGadget()}};
	QMap<QString, ChildGadget> r2 = {
		{"v3", ChildGadget()},
		{"v4", ChildGadget()},
		{"v5", ChildGadget()}};
	QMap<QString, ChildGadget> r3 = {
		{"v6", ChildGadget()},
		{"v7", ChildGadget()},
		{"v8", ChildGadget()}};
	QTest::newRow("QMap<QString, QMap<QString, ChildGadget>>") << QVariant::fromValue<QMap<QString, QMap<QString, ChildGadget>>>({
																																	 {"m1", r1},
																																	 {"m2", r2},
																																	 {"m3", r3}
																																 })
															   << (int)QVariant::Map;
}

void GadgetSerializerTest::testVariantConversions()
{
	QFETCH(QVariant, data);
	QFETCH(int, targetType);

	auto origType = data.userType();
	auto convData = data;
	QVERIFY(convData.convert(targetType));
	QVERIFY(convData.convert(origType));
	QCOMPARE(convData, data);
}

void GadgetSerializerTest::testSerialization_data()
{
	QTest::addColumn<TestGadget>("gadget");
	QTest::addColumn<QJsonObject>("result");

	generateValidTestData();
}

void GadgetSerializerTest::testSerialization()
{
	QFETCH(TestGadget, gadget);
	QFETCH(QJsonObject, result);

	try {
		qDebug() << result;
		QCOMPARE(serializer->serialize(gadget), result);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

//void GadgetSerializerTest::testInvalidSerialization()
//{
//	try {
//		QVERIFY_EXCEPTION_THROWN(serializer->serialize(BrokenTestGadget()), QJsonSerializerException);
//	} catch(QException &e) {
//		QFAIL(e.what());
//	}
//}

void GadgetSerializerTest::testDeserialization_data()
{
	QTest::addColumn<TestGadget>("gadget");
	QTest::addColumn<QJsonObject>("data");

	generateValidTestData();
}

void GadgetSerializerTest::testDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(TestGadget, gadget);

	try {
		QCOMPARE(serializer->deserialize<TestGadget>(data), gadget);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

//void GadgetSerializerTest::testInvalidDeserialization()
//{
//	QJsonObject broken({
//						   {"intProperty", 0},
//						   {"boolProperty", false},
//						   {"stringProperty", QString()},
//						   {"doubleProperty", 0},
//						   {"normalEnumProperty", TestGadget::Normal0},
//						   {"enumFlagsProperty", 0},
//						   {"simpleList", QJsonArray()},
//						   {"leveledList", QJsonArray()},
//						   {"simpleMap", QJsonObject()},
//						   {"leveledMap", QJsonObject()},
//						   {"childGadget", QJsonObject({
//								{"intProperty", 0},
//								{"boolProperty", false},
//								{"stringProperty", QString()},
//								{"doubleProperty", 0},
//								{"normalEnumProperty", TestGadget::Normal0},
//								{"enumFlagsProperty", 0},
//								{"simpleList", QJsonArray()},
//								{"leveledList", QJsonArray()}
//							})},
//						   {"simpleChildren", QJsonArray()},
//						   {"leveledChildren", QJsonArray()},
//						   {"broken", QJsonValue::Null}
//					  });

//	try {
//		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<BrokenTestGadget>(broken), QJsonSerializerException);
//	} catch(QException &e) {
//		QFAIL(e.what());
//	}
//}

//void GadgetSerializerTest::testNullChild()
//{
//	auto testGad = ParentGadget();
//	auto testJson = QJsonObject({
//									{"intProperty", 0},
//									{"boolProperty", false},
//									{"stringProperty", QString()},
//									{"doubleProperty", 0},
//									{"normalEnumProperty", TestGadget::Normal0},
//									{"enumFlagsProperty", 0},
//									{"simpleList", QJsonArray()},
//									{"leveledList", QJsonArray()},
//									{"simpleMap", QJsonObject()},
//									{"leveledMap", QJsonObject()},
//									{"childGadget", QJsonValue::Null},//this one is null here -> fails for gadget
//									{"simpleChildren", QJsonArray()},
//									{"leveledChildren", QJsonArray()}
//								});

//	try {
//		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<ParentGadget>(testJson), QJsonSerializerException);
//	} catch(QException &e) {
//		QFAIL(e.what());
//	}
//}

//void GadgetSerializerTest::testNullDeserialization()
//{
//	auto testGad = ParentGadget();
//	auto testJson = QJsonObject({
//									{"intProperty", QJsonValue::Null},
//									{"boolProperty", QJsonValue::Null},
//									{"stringProperty", QJsonValue::Null},
//									{"doubleProperty", QJsonValue::Null},
//									{"normalEnumProperty", QJsonValue::Null},
//									{"enumFlagsProperty", QJsonValue::Null},
//									{"simpleList", QJsonValue::Null},
//									{"leveledList", QJsonValue::Null},
//									{"simpleMap", QJsonValue::Null},
//									{"leveledMap", QJsonValue::Null},
//									{"childGadget", QJsonValue::Null},
//									{"simpleChildren", QJsonValue::Null},
//									{"leveledChildren", QJsonValue::Null}
//								});

//	try {
//		serializer->setAllowDefaultNull(false);
//		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<ParentGadget>(testJson), QJsonSerializerException);

//		serializer->setAllowDefaultNull(true);
//		QCOMPARE(serializer->deserialize<ParentGadget>(testJson), testGad);
//	} catch(QException &e) {
//		QFAIL(e.what());
//	}

//	serializer->setAllowDefaultNull(false);
//}

//void GadgetSerializerTest::testDeserializationValidation_data()
//{
//	QTest::addColumn<QJsonObject>("data");
//	QTest::addColumn<QJsonSerializer::ValidationFlags>("flags");
//	QTest::addColumn<bool>("success");

//	QTest::newRow("standard") << QJsonObject({
//												 {"intProperty", 0},
//												 {"boolProperty", false},
//												 {"stringProperty", QString()},
//												 {"doubleProperty", 0},
//												 {"normalEnumProperty", TestGadget::Normal0},
//												 {"enumFlagsProperty", 0},
//												 {"simpleList", QJsonArray()},
//												 {"leveledList", QJsonArray()},
//												 {"simpleMap", QJsonObject()},
//												 //{"leveledMap", QJsonObject()}, missing property
//												 {"garbage", QJsonValue::Null} //extra property
//											 })
//							  << (QJsonSerializer::ValidationFlags)QJsonSerializer::StandardValidation
//							  << true;

//	QTest::newRow("validateExtra") << QJsonObject({
//													  {"intProperty", 0},
//													  {"boolProperty", false},
//													  {"stringProperty", QString()},
//													  {"doubleProperty", 0},
//													  {"normalEnumProperty", TestGadget::Normal0},
//													  {"enumFlagsProperty", 0},
//													  {"simpleList", QJsonArray()},
//													  {"leveledList", QJsonArray()},
//													  {"simpleMap", QJsonObject()},
//													  {"leveledMap", QJsonObject()},
//													  {"garbage", QJsonValue::Null} //extra property
//												  })
//								   << (QJsonSerializer::ValidationFlags)QJsonSerializer::NoExtraProperties
//								   << false;

//	QTest::newRow("validateMissing") << QJsonObject({
//														{"intProperty", 0},
//														{"boolProperty", false},
//														{"stringProperty", QString()},
//														{"doubleProperty", 0},
//														{"normalEnumProperty", TestGadget::Normal0},
//														{"enumFlagsProperty", 0},
//														{"simpleList", QJsonArray()},
//														{"leveledList", QJsonArray()},
//														{"simpleMap", QJsonObject()}
//														//{"leveledMap", QJsonObject()}, missing property
//													})
//									 << (QJsonSerializer::ValidationFlags)QJsonSerializer::AllProperties
//									 << false;

//	QTest::newRow("validateAll") << QJsonObject({
//													{"intProperty", 0},
//													{"boolProperty", false},
//													{"stringProperty", QString()},
//													{"doubleProperty", 0},
//													{"normalEnumProperty", TestGadget::Normal0},
//													{"enumFlagsProperty", 0},
//													{"simpleList", QJsonArray()},
//													{"leveledList", QJsonArray()},
//													{"simpleMap", QJsonObject()},
//													//{"leveledMap", QJsonObject()}, missing property
//													{"garbage", QJsonValue::Null}, //extra property
//												})
//								 << (QJsonSerializer::ValidationFlags)QJsonSerializer::FullValidation
//								 << false;
//}

//void GadgetSerializerTest::testDeserializationValidation()
//{
//	QFETCH(QJsonObject, data);
//	QFETCH(QJsonSerializer::ValidationFlags, flags);
//	QFETCH(bool, success);

//	serializer->setValidationFlags(flags);

//	try {
//		if(success) {
//			auto deser = serializer->deserialize<TestGadget>(data, this);
//			QCOMPARE(deser, TestGadget());
//		} else
//			QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestGadget>(data, this), QJsonDeserializationException);
//	} catch (QException &e) {
//		QFAIL(e.what());
//	}

//	serializer->setValidationFlags(QJsonSerializer::StandardValidation);
//}

//void GadgetSerializerTest::testEnumSpecialSerialization_data()
//{
//	QTest::addColumn<TestGadget>("gadget");
//	QTest::addColumn<QJsonObject>("result");
//	QTest::addColumn<bool>("asString");

//	QTest::newRow("normal") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal1, 0)
//							<< QJsonObject({
//											   {"intProperty", 0},
//											   {"boolProperty", false},
//											   {"stringProperty", QString()},
//											   {"doubleProperty", 0},
//											   {"normalEnumProperty", TestGadget::Normal1},
//											   {"enumFlagsProperty", 0},
//											   {"simpleList", QJsonArray()},
//											   {"leveledList", QJsonArray()},
//											   {"simpleMap", QJsonObject()},
//											   {"leveledMap", QJsonObject()}
//										   })
//							<< false;

//	QTest::newRow("stringEnum") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal1, 0)
//								<< QJsonObject({
//												   {"intProperty", 0},
//												   {"boolProperty", false},
//												   {"stringProperty", QString()},
//												   {"doubleProperty", 0},
//												   {"normalEnumProperty", "Normal1"},
//												   {"enumFlagsProperty", QString()},
//												   {"simpleList", QJsonArray()},
//												   {"leveledList", QJsonArray()},
//												   {"simpleMap", QJsonObject()},
//												   {"leveledMap", QJsonObject()}
//											   })
//								<< true;

//	QTest::newRow("stringFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal0, TestGadget::FlagX)
//								<< QJsonObject({
//												   {"intProperty", 0},
//												   {"boolProperty", false},
//												   {"stringProperty", QString()},
//												   {"doubleProperty", 0},
//												   {"normalEnumProperty", "Normal0"},
//												   {"enumFlagsProperty", "FlagX"},
//												   {"simpleList", QJsonArray()},
//												   {"leveledList", QJsonArray()},
//												   {"simpleMap", QJsonObject()},
//												   {"leveledMap", QJsonObject()}
//											   })
//								<< true;

//	QTest::newRow("stringMultiFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal2, TestGadget::Flag1 | TestGadget::Flag3)
//									  << QJsonObject({
//														 {"intProperty", 0},
//														 {"boolProperty", false},
//														 {"stringProperty", QString()},
//														 {"doubleProperty", 0},
//														 {"normalEnumProperty", "Normal2"},
//														 {"enumFlagsProperty", "Flag1|Flag3"},
//														 {"simpleList", QJsonArray()},
//														 {"leveledList", QJsonArray()},
//														 {"simpleMap", QJsonObject()},
//														 {"leveledMap", QJsonObject()}
//													 })
//									  << true;
//}

//void GadgetSerializerTest::testEnumSpecialSerialization()
//{
//	QFETCH(TestGadget, gadget);
//	QFETCH(QJsonObject, result);
//	QFETCH(bool, asString);

//	try {
//		serializer->setEnumAsString(asString);
//		QCOMPARE(serializer->serialize(gadget), result);
//	} catch(QException &e) {
//		QFAIL(e.what());
//	}
//}

//void GadgetSerializerTest::testEnumSpecialDeserialization_data()
//{
//	QTest::addColumn<TestGadget>("result");
//	QTest::addColumn<QJsonObject>("data");
//	QTest::addColumn<bool>("success");

//	QTest::newRow("normal") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal1, 0)
//							<< QJsonObject({
//											   {"intProperty", 0},
//											   {"boolProperty", false},
//											   {"stringProperty", QString()},
//											   {"doubleProperty", 0},
//											   {"normalEnumProperty", TestGadget::Normal1},
//											   {"enumFlagsProperty", 0},
//											   {"simpleList", QJsonArray()},
//											   {"leveledList", QJsonArray()},
//											   {"simpleMap", QJsonObject()},
//											   {"leveledMap", QJsonObject()}
//										   })
//							<< true;

//	QTest::newRow("stringEnum") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal1, 0)
//								<< QJsonObject({
//												   {"intProperty", 0},
//												   {"boolProperty", false},
//												   {"stringProperty", QString()},
//												   {"doubleProperty", 0},
//												   {"normalEnumProperty", "Normal1"},
//												   {"enumFlagsProperty", QString()},
//												   {"simpleList", QJsonArray()},
//												   {"leveledList", QJsonArray()},
//												   {"simpleMap", QJsonObject()},
//												   {"leveledMap", QJsonObject()}
//											   })
//								<< true;

//	QTest::newRow("invalidEnum") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal2, 0)
//								 << QJsonObject({
//													{"intProperty", 0},
//													{"boolProperty", false},
//													{"stringProperty", QString()},
//													{"doubleProperty", 0},
//													{"normalEnumProperty", "baum"},
//													{"enumFlagsProperty", 0},
//													{"simpleList", QJsonArray()},
//													{"leveledList", QJsonArray()},
//													{"simpleMap", QJsonObject()},
//													{"leveledMap", QJsonObject()}
//												})
//								 << false;

//	QTest::newRow("stringFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal0, TestGadget::FlagX)
//								 << QJsonObject({
//													{"intProperty", 0},
//													{"boolProperty", false},
//													{"stringProperty", QString()},
//													{"doubleProperty", 0},
//													{"normalEnumProperty", "Normal0"},
//													{"enumFlagsProperty", "FlagX"},
//													{"simpleList", QJsonArray()},
//													{"leveledList", QJsonArray()},
//													{"simpleMap", QJsonObject()},
//													{"leveledMap", QJsonObject()}
//												})
//								 << true;

//	QTest::newRow("stringMultiFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal2, TestGadget::Flag1 | TestGadget::Flag3)
//									  << QJsonObject({
//														 {"intProperty", 0},
//														 {"boolProperty", false},
//														 {"stringProperty", QString()},
//														 {"doubleProperty", 0},
//														 {"normalEnumProperty", "Normal2"},
//														 {"enumFlagsProperty", "Flag1|Flag3"},
//														 {"simpleList", QJsonArray()},
//														 {"leveledList", QJsonArray()},
//														 {"simpleMap", QJsonObject()},
//														 {"leveledMap", QJsonObject()}
//													 })
//									  << true;

//	QTest::newRow("invalidFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal0, TestGadget::FlagX)
//								  << QJsonObject({
//													 {"intProperty", 0},
//													 {"boolProperty", false},
//													 {"stringProperty", QString()},
//													 {"doubleProperty", 0},
//													 {"normalEnumProperty", 0},
//													 {"enumFlagsProperty", "baum"},
//													 {"simpleList", QJsonArray()},
//													 {"leveledList", QJsonArray()},
//													 {"simpleMap", QJsonObject()},
//													 {"leveledMap", QJsonObject()}
//												 })
//								  << false;
//}

//void GadgetSerializerTest::testEnumSpecialDeserialization()
//{
//	QFETCH(QJsonObject, data);
//	QFETCH(TestGadget, result);
//	QFETCH(bool, success);

//	try {
//		if(success) {
//			auto obj = serializer->deserialize<TestGadget>(data, this);
//			QCOMPARE(obj, result);
//		} else
//			QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestGadget>(data, this), QJsonDeserializationException);
//	} catch(QException &e) {
//		QFAIL(e.what());
//	}
//}

//void GadgetSerializerTest::testDeviceSerialization_data()
//{
//	QTest::addColumn<QVariant>("data");
//	QTest::addColumn<QByteArray>("fakeDevice");
//	QTest::addColumn<bool>("works");

//	QTest::newRow("object") << QVariant::fromValue((TestGadget)ParentGadget::createBasic(42, true, "baum", 4.2))
//							<< QByteArray()
//							<< true;

//	QTest::newRow("list") << QVariant::fromValue(QList<TestGadget>({ParentGadget::createBasic(42, true, "baum", 4.2), ParentGadget()}))
//						  << QByteArray()
//						  << true;

//	QTest::newRow("data") << QVariant(42)
//						  << QByteArray()
//						  << false;

//	QTest::newRow("object") << QVariant::fromValue(ParentGadget())
//							<< QByteArray("invalid stuff")
//							<< true;
//}

//void GadgetSerializerTest::testDeviceSerialization()
//{
//	QFETCH(QVariant, data);
//	QFETCH(QByteArray, fakeDevice);
//	QFETCH(bool, works);

//	try {
//		QTemporaryFile tFile;
//		QVERIFY(tFile.open());
//		if(works)
//			serializer->serializeTo(&tFile, data);
//		else {
//			QVERIFY_EXCEPTION_THROWN(serializer->serializeTo(&tFile, data), QJsonSerializerException);
//			return;
//		}

//		tFile.close();
//		if(!fakeDevice.isEmpty()){
//			QBuffer buffer(&fakeDevice);
//			buffer.open(QIODevice::ReadOnly);
//			QVERIFY_EXCEPTION_THROWN(serializer->deserializeFrom(&buffer, data.userType(), this), QJsonSerializerException);
//		} else {
//			QVERIFY(tFile.open());
//			auto res = serializer->deserializeFrom(&tFile, data.userType(), this);
//			QCOMPARE(res, data);
//		}

//		tFile.close();
//	} catch(QException &e) {
//		QFAIL(e.what());
//	}
//}

void GadgetSerializerTest::generateValidTestData()
{
	QTest::newRow("default") << TestGadget()
							 << TestGadget::createJson();

	QTest::newRow("basic") << TestGadget::createBasic(42, true, "baum", 4.2)
						   << TestGadget::createJson({
														 {"intProperty", 42},
														 {"boolProperty", true},
														 {"stringProperty", QStringLiteral("baum")},
														 {"doubleProperty", 4.2}
													 });

	QTest::newRow("enum") << TestGadget::createEnum(TestGadget::Normal2, 0)
						  << TestGadget::createJson({
														{"normalEnumProperty", TestGadget::Normal2}
													});
	QTest::newRow("flags") << TestGadget::createEnum(TestGadget::Normal0, TestGadget::Flag1 | TestGadget::Flag2)
						   << TestGadget::createJson({
														 {"enumFlagsProperty", TestGadget::FlagX}
													 });

	QTest::newRow("list") << TestGadget::createList({3, 7, 13}, {})
						  << TestGadget::createJson({
														{"simpleList", QJsonArray({3, 7, 13})}
													});

	{
		QList<int> l1 = {0, 1, 2};
		QList<int> l2 = {3, 4, 5};
		QList<int> l3 = {6, 7, 8};
		QJsonArray j1 = {0, 1, 2};
		QJsonArray j2 = {3, 4, 5};
		QJsonArray j3 = {6, 7, 8};
		QTest::newRow("list<list>") << TestGadget::createList({}, {l1, l2, l3})
									<< TestGadget::createJson({
																  {"leveledList", QJsonArray({j1, j2, j3})}
															  });
	}

	QTest::newRow("map") << TestGadget::createMap({{"v3", 3}, {"v7", 7}, {"v13", 13}}, {})
						 << TestGadget::createJson({
													   {"simpleMap", QJsonObject({
															{"v3", 3},
															{"v7", 7},
															{"v13", 13}
														})}
												   });

	{
		QMap<QString, int> m1 = {{"v0", 0}, {"v1", 1}, {"v2", 2}};
		QMap<QString, int> m2 = {{"v3", 3}, {"v4", 4}, {"v5", 5}};
		QMap<QString, int> m3 = {{"v6", 6}, {"v7", 7}, {"v8", 8}};
		QJsonObject j1 = {{"v0", 0}, {"v1", 1}, {"v2", 2}};
		QJsonObject j2 = {{"v3", 3}, {"v4", 4}, {"v5", 5}};
		QJsonObject j3 = {{"v6", 6}, {"v7", 7}, {"v8", 8}};
		QTest::newRow("map<map>") << TestGadget::createMap({}, {{"m1", m1}, {"m2", m2}, {"m3", m3}})
								  << TestGadget::createJson({
																{"leveledMap", QJsonObject({
																	 {"m1", j1},
																	 {"m2", j2},
																	 {"m3", j3}
																 })}
															});
	}

	QTest::newRow("child") << TestGadget::createChild(ChildGadget(77))
						   << TestGadget::createJson({
											  {"childGadget", ChildGadget::createJson(77)}
										  });

	QTest::newRow("childlist") << TestGadget::createChildren({ChildGadget(3), ChildGadget(7), ChildGadget(13)}, {})
							   << TestGadget::createJson({
															 {"simpleChildren", QJsonArray({
																  ChildGadget::createJson(3),
																  ChildGadget::createJson(7),
																  ChildGadget::createJson(13),
															  })},
														 });

	{
		QList<QList<ChildGadget>> ccl = {
			{ChildGadget(0), ChildGadget(1), ChildGadget(2)},
			{ChildGadget(3), ChildGadget(4), ChildGadget(5)},
			{ChildGadget(6), ChildGadget(7), ChildGadget(8)}
		};
		QJsonArray jjl = {
			QJsonArray({ChildGadget::createJson(0), ChildGadget::createJson(1), ChildGadget::createJson(2)}),
			QJsonArray({ChildGadget::createJson(3), ChildGadget::createJson(4), ChildGadget::createJson(5)}),
			QJsonArray({ChildGadget::createJson(6), ChildGadget::createJson(7), ChildGadget::createJson(8)})
		};
		QTest::newRow("list<childlist>") << TestGadget::createChildren({}, ccl)
										 << TestGadget::createJson({
																	   {"leveledChildren", jjl}
																   });
	}

	QTest::newRow("childmap") << TestGadget::createRelatives({
																 {"c1", ChildGadget(3)},
																 {"c2", ChildGadget(7)},
																 {"c3", ChildGadget(13)}
															 }, {})
							  << TestGadget::createJson({
															{"simpleRelatives", QJsonObject({
																 {"c1", ChildGadget::createJson(3)},
																 {"c2", ChildGadget::createJson(7)},
																 {"c3", ChildGadget::createJson(13)},
															 })},
														});

	{
		QMap<QString, ChildGadget> cm1 = {
			{"c0", ChildGadget(0)},
			{"c1", ChildGadget(1)},
			{"c2", ChildGadget(2)}
		};
		QMap<QString, ChildGadget> cm2 = {
			{"c3", ChildGadget(3)},
			{"c4", ChildGadget(4)},
			{"c5", ChildGadget(5)}
		};
		QMap<QString, ChildGadget> cm3 = {
			{"c6", ChildGadget(6)},
			{"c7", ChildGadget(7)},
			{"c8", ChildGadget(8)}
		};
		QMap<QString, QMap<QString, ChildGadget>> ccm = {
			{"cm1", cm1},
			{"cm2", cm2},
			{"cm3", cm3}
		};

		QJsonObject j1({
			{"c0", ChildGadget::createJson(0)},
			{"c1", ChildGadget::createJson(1)},
			{"c2", ChildGadget::createJson(2)}
		});
		QJsonObject j2({
			{"c3", ChildGadget::createJson(3)},
			{"c4", ChildGadget::createJson(4)},
			{"c5", ChildGadget::createJson(5)},
		});
		QJsonObject j3({
			{"c6", ChildGadget::createJson(6)},
			{"c7", ChildGadget::createJson(7)},
			{"c8", ChildGadget::createJson(8)},
		});
		QJsonObject jjm = {
			{"cm1", j1},
			{"cm2", j2},
			{"cm3", j3}
		};
		QTest::newRow("map<childmap>") << TestGadget::createRelatives({}, ccm)
									   << TestGadget::createJson({
																	 {"leveledRelatives", jjm}
																 });
	}
}

//static void compile_test()
//{
//	QJsonSerializer s;
//	QVariant v;
//	TestGadget t;
//	QList<TestGadget> l;
//	QMap<QString, TestGadget> m;
//	QIODevice *d = nullptr;
//	QByteArray b;
//	QJsonValue jv;
//	QJsonObject jo;
//	QJsonArray ja;
//	QObject *p = nullptr;

//	jv = s.serialize(v);
//	jo = s.serialize(t);
//	ja = s.serialize(l);
//	jo = s.serialize(m);

//	s.serializeTo(d, v);
//	s.serializeTo(d, t);
//	s.serializeTo(d, l);
//	s.serializeTo(d, m);

//	b = s.serializeTo(v);
//	b = s.serializeTo(t);
//	b = s.serializeTo(l);
//	b = s.serializeTo(m);

//	v = s.deserialize(jv, qMetaTypeId<TestGadget>());
//	v = s.deserialize(jv, qMetaTypeId<TestGadget>(), p);
//	t = s.deserialize<TestGadget>(jo);
//	t = s.deserialize<TestGadget>(jo, p);
//	l = s.deserialize<QList<TestGadget>>(ja);
//	l = s.deserialize<QList<TestGadget>>(ja, p);
//	m = s.deserialize<QMap<QString, TestGadget>>(jo);
//	m = s.deserialize<QMap<QString, TestGadget>>(jo, p);

//	v = s.deserializeFrom(d, qMetaTypeId<TestGadget>());
//	v = s.deserializeFrom(d, qMetaTypeId<TestGadget>(), p);
//	t = s.deserializeFrom<TestGadget>(d);
//	t = s.deserializeFrom<TestGadget>(d, p);
//	l = s.deserializeFrom<QList<TestGadget>>(d);
//	l = s.deserializeFrom<QList<TestGadget>>(d, p);
//	m = s.deserializeFrom<QMap<QString, TestGadget>>(d);
//	m = s.deserializeFrom<QMap<QString, TestGadget>>(d, p);

//	v = s.deserializeFrom(b, qMetaTypeId<TestGadget>());
//	v = s.deserializeFrom(b, qMetaTypeId<TestGadget>(), p);
//	t = s.deserializeFrom<TestGadget>(b);
//	t = s.deserializeFrom<TestGadget>(b, p);
//	l = s.deserializeFrom<QList<TestGadget>>(b);
//	l = s.deserializeFrom<QList<TestGadget>>(b, p);
//	m = s.deserializeFrom<QMap<QString, TestGadget>>(b);
//	m = s.deserializeFrom<QMap<QString, TestGadget>>(b, p);
//}

QTEST_MAIN(GadgetSerializerTest)

#include "tst_gadgetserializer.moc"
