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
	void testInvalidSerialization();
	void testDeserialization_data();
	void testDeserialization();
	void testInvalidDeserialization();

	void testNullChild();
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

void GadgetSerializerTest::initTestCase()
{
#ifdef Q_OS_LINUX
	Q_ASSERT(qgetenv("LD_PRELOAD").contains("Qt5JsonSerializer"));
#endif
	QJsonSerializer::registerListConverters<QList<int>>();
	QJsonSerializer::registerListConverters<TestGadget>();
	QJsonSerializer::registerListConverters<QList<TestGadget>>();
	QJsonSerializer::registerMapConverters<QMap<QString, int>>();
	//register list comparators, needed for test only!
	QMetaType::registerComparators<TestGadget>();
	QMetaType::registerComparators<QList<int>>();
	QMetaType::registerComparators<QList<QList<int>>>();
	QMetaType::registerComparators<QMap<QString, int>>();
	QMetaType::registerComparators<QMap<QString, QMap<QString, int>>>();
	QMetaType::registerComparators<QList<TestGadget>>();
	QMetaType::registerComparators<QList<QList<TestGadget>>>();
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

	QTest::newRow("QList<TestGadget>") << QVariant::fromValue<QList<TestGadget>>({
																					   TestGadget(),
																					   TestGadget(),
																					   TestGadget()
																				   })
										<< (int)QVariant::List;
	QList<TestGadget> o1 = {
		TestGadget(),
		TestGadget(),
		TestGadget()
	};
	QList<TestGadget> o2 = {
		TestGadget(),
		TestGadget(),
		TestGadget()
	};
	QList<TestGadget> o3 = {
		TestGadget(),
		TestGadget(),
		TestGadget()
	};
	QTest::newRow("QList<QList<TestGadget>>") << QVariant::fromValue<QList<QList<TestGadget>>>({o1, o2, o3})
											   << (int)QVariant::List;
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
	QTest::addColumn<ParentGadget>("gadget");
	QTest::addColumn<QJsonObject>("result");

	generateValidTestData();
}

void GadgetSerializerTest::testSerialization()
{
	QFETCH(ParentGadget, gadget);
	QFETCH(QJsonObject, result);

	try {
		QCOMPARE(serializer->serialize(gadget), result);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testInvalidSerialization()
{
	try {
		QVERIFY_EXCEPTION_THROWN(serializer->serialize(BrokenTestGadget()), QJsonSerializerException);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testDeserialization_data()
{
	QTest::addColumn<ParentGadget>("gadget");
	QTest::addColumn<QJsonObject>("data");

	generateValidTestData();
}

void GadgetSerializerTest::testDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(ParentGadget, gadget);

	try {
		QCOMPARE(serializer->deserialize<ParentGadget>(data), gadget);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testInvalidDeserialization()
{
	QJsonObject broken({
						   {"intProperty", 0},
						   {"boolProperty", false},
						   {"stringProperty", QString()},
						   {"doubleProperty", 0},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()},
						   {"childGadget", QJsonObject({
								{"intProperty", 0},
								{"boolProperty", false},
								{"stringProperty", QString()},
								{"doubleProperty", 0},
								{"normalEnumProperty", TestGadget::Normal0},
								{"enumFlagsProperty", 0},
								{"simpleList", QJsonArray()},
								{"leveledList", QJsonArray()}
							})},
						   {"simpleChildren", QJsonArray()},
						   {"leveledChildren", QJsonArray()},
						   {"broken", QJsonValue::Null}
					  });

	try {
		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<BrokenTestGadget>(broken), QJsonSerializerException);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testNullChild()
{
	auto testGad = ParentGadget();
	auto testJson = QJsonObject({
									{"intProperty", 0},
									{"boolProperty", false},
									{"stringProperty", QString()},
									{"doubleProperty", 0},
									{"normalEnumProperty", TestGadget::Normal0},
									{"enumFlagsProperty", 0},
									{"simpleList", QJsonArray()},
									{"leveledList", QJsonArray()},
									{"simpleMap", QJsonObject()},
									{"leveledMap", QJsonObject()},
									{"childGadget", QJsonValue::Null},//this one is null here -> fails for gadget
									{"simpleChildren", QJsonArray()},
									{"leveledChildren", QJsonArray()}
								});

	try {
		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<ParentGadget>(testJson), QJsonSerializerException);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testNullDeserialization()
{
	auto testGad = ParentGadget();
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
									{"childGadget", QJsonValue::Null},
									{"simpleChildren", QJsonValue::Null},
									{"leveledChildren", QJsonValue::Null}
								});

	try {
		serializer->setAllowDefaultNull(false);
		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<ParentGadget>(testJson), QJsonSerializerException);

		serializer->setAllowDefaultNull(true);
		QCOMPARE(serializer->deserialize<ParentGadget>(testJson), testGad);
	} catch(QException &e) {
		QFAIL(e.what());
	}

	serializer->setAllowDefaultNull(false);
}

void GadgetSerializerTest::testEnumSpecialSerialization_data()
{
	QTest::addColumn<TestGadget>("gadget");
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<bool>("asString");

	QTest::newRow("normal") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal1, 0)
							<< QJsonObject({
											   {"intProperty", 0},
											   {"boolProperty", false},
											   {"stringProperty", QString()},
											   {"doubleProperty", 0},
											   {"normalEnumProperty", TestGadget::Normal1},
											   {"enumFlagsProperty", 0},
											   {"simpleList", QJsonArray()},
											   {"leveledList", QJsonArray()},
											   {"simpleMap", QJsonObject()},
											   {"leveledMap", QJsonObject()}
										   })
							<< false;

	QTest::newRow("stringEnum") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal1, 0)
								<< QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", "Normal1"},
												   {"enumFlagsProperty", QString()},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"simpleMap", QJsonObject()},
												   {"leveledMap", QJsonObject()}
											   })
								<< true;

	QTest::newRow("stringFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal0, TestGadget::FlagX)
								<< QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", "Normal0"},
												   {"enumFlagsProperty", "FlagX"},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"simpleMap", QJsonObject()},
												   {"leveledMap", QJsonObject()}
											   })
								<< true;

	QTest::newRow("stringMultiFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal2, TestGadget::Flag1 | TestGadget::Flag3)
									  << QJsonObject({
														 {"intProperty", 0},
														 {"boolProperty", false},
														 {"stringProperty", QString()},
														 {"doubleProperty", 0},
														 {"normalEnumProperty", "Normal2"},
														 {"enumFlagsProperty", "Flag1|Flag3"},
														 {"simpleList", QJsonArray()},
														 {"leveledList", QJsonArray()},
														 {"simpleMap", QJsonObject()},
														 {"leveledMap", QJsonObject()}
													 })
									  << true;
}

void GadgetSerializerTest::testEnumSpecialSerialization()
{
	QFETCH(TestGadget, gadget);
	QFETCH(QJsonObject, result);
	QFETCH(bool, asString);

	try {
		serializer->setEnumAsString(asString);
		QCOMPARE(serializer->serialize(gadget), result);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testEnumSpecialDeserialization_data()
{
	QTest::addColumn<TestGadget>("result");
	QTest::addColumn<QJsonObject>("data");

	QTest::newRow("normal") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal1, 0)
							<< QJsonObject({
											   {"intProperty", 0},
											   {"boolProperty", false},
											   {"stringProperty", QString()},
											   {"doubleProperty", 0},
											   {"normalEnumProperty", TestGadget::Normal1},
											   {"enumFlagsProperty", 0},
											   {"simpleList", QJsonArray()},
											   {"leveledList", QJsonArray()},
											   {"simpleMap", QJsonObject()},
											   {"leveledMap", QJsonObject()}
										   });

	QTest::newRow("stringEnum") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal1, 0)
								<< QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", "Normal1"},
												   {"enumFlagsProperty", QString()},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"simpleMap", QJsonObject()},
												   {"leveledMap", QJsonObject()}
											   });

	QTest::newRow("stringFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal0, TestGadget::FlagX)
								<< QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", "Normal0"},
												   {"enumFlagsProperty", "FlagX"},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"simpleMap", QJsonObject()},
												   {"leveledMap", QJsonObject()}
											   });

	QTest::newRow("stringMultiFlags") << (TestGadget)ParentGadget::createEnum(TestGadget::Normal2, TestGadget::Flag1 | TestGadget::Flag3)
									  << QJsonObject({
														 {"intProperty", 0},
														 {"boolProperty", false},
														 {"stringProperty", QString()},
														 {"doubleProperty", 0},
														 {"normalEnumProperty", "Normal2"},
														 {"enumFlagsProperty", "Flag1|Flag3"},
														 {"simpleList", QJsonArray()},
														 {"leveledList", QJsonArray()},
														 {"simpleMap", QJsonObject()},
														 {"leveledMap", QJsonObject()}
													 });
}

void GadgetSerializerTest::testEnumSpecialDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(TestGadget, result);

	try {
		auto obj = serializer->deserialize<TestGadget>(data, this);
		QCOMPARE(obj, result);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testDeviceSerialization_data()
{
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QByteArray>("fakeDevice");
	QTest::addColumn<bool>("works");

	QTest::newRow("object") << QVariant::fromValue((TestGadget)ParentGadget::createBasic(42, true, "baum", 4.2))
							<< QByteArray()
							<< true;

	QTest::newRow("list") << QVariant::fromValue(QList<TestGadget>({ParentGadget::createBasic(42, true, "baum", 4.2), ParentGadget()}))
						  << QByteArray()
						  << true;

	QTest::newRow("data") << QVariant(42)
						  << QByteArray()
						  << false;

	QTest::newRow("object") << QVariant::fromValue(ParentGadget())
							<< QByteArray("invalid stuff")
							<< true;
}

void GadgetSerializerTest::testDeviceSerialization()
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
			QCOMPARE(res, data);
		}

		tFile.close();
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::generateValidTestData()
{
	QTest::newRow("default") << ParentGadget()
							 << QJsonObject({
												{"intProperty", 0},
												{"boolProperty", false},
												{"stringProperty", QString()},
												{"doubleProperty", 0},
												{"normalEnumProperty", TestGadget::Normal0},
												{"enumFlagsProperty", 0},
												{"simpleList", QJsonArray()},
												{"leveledList", QJsonArray()},
												{"simpleMap", QJsonObject()},
												{"leveledMap", QJsonObject()},
												{"childGadget", QJsonObject({
													 {"intProperty", 0},
													 {"boolProperty", false},
													 {"stringProperty", QString()},
													 {"doubleProperty", 0},
													 {"normalEnumProperty", TestGadget::Normal0},
													 {"enumFlagsProperty", 0},
													 {"simpleList", QJsonArray()},
													 {"leveledList", QJsonArray()},
													 {"simpleMap", QJsonObject()},
													 {"leveledMap", QJsonObject()}
												 })},
												{"simpleChildren", QJsonArray()},
												{"leveledChildren", QJsonArray()}
											});

	QTest::newRow("basic") << ParentGadget::createBasic(42, true, "baum", 4.2)
						   << QJsonObject({
											  {"intProperty", 42},
											  {"boolProperty", true},
											  {"stringProperty", QStringLiteral("baum")},
											  {"doubleProperty", 4.2},
											  {"normalEnumProperty", TestGadget::Normal0},
											  {"enumFlagsProperty", 0},
											  {"simpleList", QJsonArray()},
											  {"leveledList", QJsonArray()},
											  {"simpleMap", QJsonObject()},
											  {"leveledMap", QJsonObject()},
											  {"childGadget", QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", TestGadget::Normal0},
												   {"enumFlagsProperty", 0},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"simpleMap", QJsonObject()},
												   {"leveledMap", QJsonObject()}
											   })},
											  {"simpleChildren", QJsonArray()},
											  {"leveledChildren", QJsonArray()}
										  });

	QTest::newRow("enum") << ParentGadget::createEnum(TestGadget::Normal2, 0)
						  << QJsonObject({
											 {"intProperty", 0},
											 {"boolProperty", false},
											 {"stringProperty", QString()},
											 {"doubleProperty", 0},
											 {"normalEnumProperty", TestGadget::Normal2},
											 {"enumFlagsProperty", 0},
											 {"simpleList", QJsonArray()},
											 {"leveledList", QJsonArray()},
											 {"simpleMap", QJsonObject()},
											 {"leveledMap", QJsonObject()},
											 {"childGadget", QJsonObject({
												  {"intProperty", 0},
												  {"boolProperty", false},
												  {"stringProperty", QString()},
												  {"doubleProperty", 0},
												  {"normalEnumProperty", TestGadget::Normal0},
												  {"enumFlagsProperty", 0},
												  {"simpleList", QJsonArray()},
												  {"leveledList", QJsonArray()},
												  {"simpleMap", QJsonObject()},
												  {"leveledMap", QJsonObject()}
											  })},
											 {"simpleChildren", QJsonArray()},
											 {"leveledChildren", QJsonArray()}
										 });
	QTest::newRow("flags") << ParentGadget::createEnum(TestGadget::Normal0, TestGadget::Flag1 | TestGadget::Flag2)
						   << QJsonObject({
											  {"intProperty", 0},
											  {"boolProperty", false},
											  {"stringProperty", QString()},
											  {"doubleProperty", 0},
											  {"normalEnumProperty", TestGadget::Normal0},
											  {"enumFlagsProperty", TestGadget::FlagX},
											  {"simpleList", QJsonArray()},
											  {"leveledList", QJsonArray()},
											  {"simpleMap", QJsonObject()},
											  {"leveledMap", QJsonObject()},
											  {"childGadget", QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"normalEnumProperty", TestGadget::Normal0},
												   {"enumFlagsProperty", 0},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"simpleMap", QJsonObject()},
												   {"leveledMap", QJsonObject()}
											   })},
											  {"simpleChildren", QJsonArray()},
											  {"leveledChildren", QJsonArray()}
										  });

	QTest::newRow("list") << ParentGadget::createList({3, 7, 13}, {})
						  << QJsonObject({
											 {"intProperty", 0},
											 {"boolProperty", false},
											 {"stringProperty", QString()},
											 {"doubleProperty", 0},
											 {"normalEnumProperty", TestGadget::Normal0},
											 {"enumFlagsProperty", 0},
											 {"simpleList", QJsonArray({3, 7, 13})},
											 {"leveledList", QJsonArray()},
											 {"simpleMap", QJsonObject()},
											 {"leveledMap", QJsonObject()},
											 {"childGadget", QJsonObject({
												  {"intProperty", 0},
												  {"boolProperty", false},
												  {"stringProperty", QString()},
												  {"doubleProperty", 0},
												  {"normalEnumProperty", TestGadget::Normal0},
												  {"enumFlagsProperty", 0},
												  {"simpleList", QJsonArray()},
												  {"leveledList", QJsonArray()},
												  {"simpleMap", QJsonObject()},
												  {"leveledMap", QJsonObject()}
											  })},
											 {"simpleChildren", QJsonArray()},
											 {"leveledChildren", QJsonArray()}
										 });

	{
		QList<int> l1 = {0, 1, 2};
		QList<int> l2 = {3, 4, 5};
		QList<int> l3 = {6, 7, 8};
		QJsonArray j1 = {0, 1, 2};
		QJsonArray j2 = {3, 4, 5};
		QJsonArray j3 = {6, 7, 8};
		QTest::newRow("list<list>") << ParentGadget::createList({3, 7, 13}, {l1, l2, l3})
									<< QJsonObject({
													   {"intProperty", 0},
													   {"boolProperty", false},
													   {"stringProperty", QString()},
													   {"doubleProperty", 0},
													   {"normalEnumProperty", TestGadget::Normal0},
													   {"enumFlagsProperty", 0},
													   {"simpleList", QJsonArray({3, 7, 13})},
													   {"leveledList", QJsonArray({j1, j2, j3})},
													   {"simpleMap", QJsonObject()},
													   {"leveledMap", QJsonObject()},
													   {"childGadget", QJsonObject({
															{"intProperty", 0},
															{"boolProperty", false},
															{"stringProperty", QString()},
															{"doubleProperty", 0},
															{"normalEnumProperty", TestGadget::Normal0},
															{"enumFlagsProperty", 0},
															{"simpleList", QJsonArray()},
															{"leveledList", QJsonArray()},
															{"simpleMap", QJsonObject()},
															{"leveledMap", QJsonObject()}
														})},
													   {"simpleChildren", QJsonArray()},
													   {"leveledChildren", QJsonArray()}
												   });
	}

	QTest::newRow("child") << ParentGadget::createChild(ParentGadget::createBasic(42, true, "baum", 4.2), {}, {})
						   << QJsonObject({
											  {"intProperty", 0},
											  {"boolProperty", false},
											  {"stringProperty", QString()},
											  {"doubleProperty", 0},
											  {"normalEnumProperty", TestGadget::Normal0},
											  {"enumFlagsProperty", 0},
											  {"simpleList", QJsonArray()},
											  {"leveledList", QJsonArray()},
											  {"simpleMap", QJsonObject()},
											  {"leveledMap", QJsonObject()},
											  {"childGadget", QJsonObject({
												   {"intProperty", 42},
												   {"boolProperty", true},
												   {"stringProperty", QStringLiteral("baum")},
												   {"doubleProperty", 4.2},
												   {"normalEnumProperty", TestGadget::Normal0},
												   {"enumFlagsProperty", 0},
												   {"simpleList", QJsonArray()},
												   {"leveledList", QJsonArray()},
												   {"simpleMap", QJsonObject()},
												   {"leveledMap", QJsonObject()}
											   })},
											  {"simpleChildren", QJsonArray()},
											  {"leveledChildren", QJsonArray()}
										  });

	{
		TestGadget c1 = ParentGadget::createBasic(1, false, "1", 1.1);
		TestGadget c2 = ParentGadget::createBasic(2, true, "2", 2.2);
		TestGadget c3 = ParentGadget::createBasic(3, false, "3", 3.3);
		QJsonObject j1({
						   {"intProperty", 1},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("1")},
						   {"doubleProperty", 1.1},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j2({
						   {"intProperty", 2},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("2")},
						   {"doubleProperty", 2.2},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j3({
						   {"intProperty", 3},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("3")},
						   {"doubleProperty", 3.3},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QTest::newRow("childlist") << ParentGadget::createChild(TestGadget(), {c1, c2, c3}, {})
								   << QJsonObject({
													  {"intProperty", 0},
													  {"boolProperty", false},
													  {"stringProperty", QString()},
													  {"doubleProperty", 0},
													  {"normalEnumProperty", TestGadget::Normal0},
													  {"enumFlagsProperty", 0},
													  {"simpleList", QJsonArray()},
													  {"leveledList", QJsonArray()},
													  {"simpleMap", QJsonObject()},
													  {"leveledMap", QJsonObject()},
													  {"childGadget", QJsonObject({
														   {"intProperty", 0},
														   {"boolProperty", false},
														   {"stringProperty", QString()},
														   {"doubleProperty", 0},
														   {"normalEnumProperty", TestGadget::Normal0},
														   {"enumFlagsProperty", 0},
														   {"simpleList", QJsonArray()},
														   {"leveledList", QJsonArray()},
														   {"simpleMap", QJsonObject()},
														   {"leveledMap", QJsonObject()}
													   })},
													  {"simpleChildren", QJsonArray({j1, j2, j3})},
													  {"leveledChildren", QJsonArray()}
												  });
	}

	{
		TestGadget c0 = ParentGadget::createBasic(0, false, "0", 0.0);
		TestGadget c1 = ParentGadget::createBasic(1, true, "1", 1.1);
		TestGadget c2 = ParentGadget::createBasic(2, false, "2", 2.2);
		TestGadget c3 = ParentGadget::createBasic(3, true, "3", 3.3);
		TestGadget c4 = ParentGadget::createBasic(4, false, "4", 4.4);
		TestGadget c5 = ParentGadget::createBasic(5, true, "5", 5.5);
		TestGadget c6 = ParentGadget::createBasic(6, false, "6", 6.6);
		TestGadget c7 = ParentGadget::createBasic(7, true, "7", 7.7);
		TestGadget c8 = ParentGadget::createBasic(8, false, "8", 8.8);
		QList<QList<TestGadget>> ccl = {
			{c0, c1, c2},
			{c3, c4, c5},
			{c6, c7, c8}
		};
		QJsonObject j0({
						   {"intProperty", 0},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("0")},
						   {"doubleProperty", 0.0},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j1({
						   {"intProperty", 1},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("1")},
						   {"doubleProperty", 1.1},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j2({
						   {"intProperty", 2},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("2")},
						   {"doubleProperty", 2.2},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j3({
						   {"intProperty", 3},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("3")},
						   {"doubleProperty", 3.3},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j4({
						   {"intProperty", 4},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("4")},
						   {"doubleProperty", 4.4},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j5({
						   {"intProperty", 5},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("5")},
						   {"doubleProperty", 5.5},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j6({
						   {"intProperty", 6},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("6")},
						   {"doubleProperty", 6.6},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j7({
						   {"intProperty", 7},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("7")},
						   {"doubleProperty", 7.7},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonObject j8({
						   {"intProperty", 8},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("8")},
						   {"doubleProperty", 8.8},
						   {"normalEnumProperty", TestGadget::Normal0},
						   {"enumFlagsProperty", 0},
						   {"simpleList", QJsonArray()},
						   {"leveledList", QJsonArray()},
						   {"simpleMap", QJsonObject()},
						   {"leveledMap", QJsonObject()}
					   });
		QJsonArray jjl = {
			QJsonArray({j0, j1, j2}),
			QJsonArray({j3, j4, j5}),
			QJsonArray({j6, j7, j8})
		};
		QTest::newRow("list<childlist>") << ParentGadget::createChild(TestGadget(), {}, ccl)
										 << QJsonObject({
															{"intProperty", 0},
															{"boolProperty", false},
															{"stringProperty", QString()},
															{"doubleProperty", 0},
															{"normalEnumProperty", TestGadget::Normal0},
															{"enumFlagsProperty", 0},
															{"simpleList", QJsonArray()},
															{"leveledList", QJsonArray()},
															{"simpleMap", QJsonObject()},
															{"leveledMap", QJsonObject()},
															{"childGadget", QJsonObject({
																 {"intProperty", 0},
																 {"boolProperty", false},
																 {"stringProperty", QString()},
																 {"doubleProperty", 0},
																 {"normalEnumProperty", TestGadget::Normal0},
																 {"enumFlagsProperty", 0},
																 {"simpleList", QJsonArray()},
																 {"leveledList", QJsonArray()},
																 {"simpleMap", QJsonObject()},
																 {"leveledMap", QJsonObject()}
															 })},
															{"simpleChildren", QJsonArray()},
															{"leveledChildren", jjl}
														});
	}
}

static void compile_test()
{
	QJsonSerializer s;
	QVariant v;
	TestGadget t;
	QList<TestGadget> l;
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

	v = s.deserialize(jv, qMetaTypeId<TestGadget>());
	v = s.deserialize(jv, qMetaTypeId<TestGadget>(), p);
	t = s.deserialize<TestGadget>(jo);
	t = s.deserialize<TestGadget>(jo, p);
	l = s.deserialize<QList<TestGadget>>(ja);
	l = s.deserialize<QList<TestGadget>>(ja, p);

	v = s.deserializeFrom(d, qMetaTypeId<TestGadget>());
	v = s.deserializeFrom(d, qMetaTypeId<TestGadget>(), p);
	t = s.deserializeFrom<TestGadget>(d);
	t = s.deserializeFrom<TestGadget>(d, p);
	l = s.deserializeFrom<QList<TestGadget>>(d);
	l = s.deserializeFrom<QList<TestGadget>>(d, p);

	v = s.deserializeFrom(b, qMetaTypeId<TestGadget>());
	v = s.deserializeFrom(b, qMetaTypeId<TestGadget>(), p);
	t = s.deserializeFrom<TestGadget>(b);
	t = s.deserializeFrom<TestGadget>(b, p);
	l = s.deserializeFrom<QList<TestGadget>>(b);
	l = s.deserializeFrom<QList<TestGadget>>(b, p);
}

QTEST_MAIN(GadgetSerializerTest)

#include "tst_gadgetserializer.moc"
