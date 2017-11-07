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
	void testDeserializationValidation_data();
	void testDeserializationValidation();

	void testEnumSpecialSerialization_data();
	void testEnumSpecialSerialization();
	void testEnumSpecialDeserialization_data();
	void testEnumSpecialDeserialization();

	void testDeviceSerialization_data();
	void testDeviceSerialization();

	void testExceptionTrace();

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

	QJsonSerializer::registerAllConverters<TestGadget>();

	QJsonSerializer::registerPairConverters<int, QString>();
	QJsonSerializer::registerPairConverters<bool, bool>();
	QJsonSerializer::registerPairConverters<ChildGadget, QList<int>>();
	QJsonSerializer::registerListConverters<QPair<bool, bool>>();

	//register list comparators, needed for test only!
	QMetaType::registerComparators<QList<int>>();
	QMetaType::registerComparators<QList<QList<int>>>();
	QMetaType::registerComparators<QMap<QString, int>>();
	QMetaType::registerComparators<QMap<QString, QMap<QString, int>>>();

	QMetaType::registerComparators<ChildGadget>();
	QMetaType::registerComparators<QList<ChildGadget>>();
	QMetaType::registerComparators<QList<QList<ChildGadget>>>();
	QMetaType::registerComparators<QMap<QString, ChildGadget>>();
	QMetaType::registerComparators<QMap<QString, QMap<QString, ChildGadget>>>();

	QMetaType::registerComparators<TestGadget>();
	QMetaType::registerComparators<QList<TestGadget>>();

	QMetaType::registerComparators<QPair<int, QString>>();
	QMetaType::registerComparators<QPair<ChildGadget, QList<int>>>();
	QMetaType::registerComparators<QList<QPair<bool, bool>>>();

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

	QTest::newRow("QPair<int, QString>") << QVariant::fromValue<QPair<int, QString>>({42, "baum"})
										 << qMetaTypeId<QPair<QVariant, QVariant>>();

	QTest::newRow("QPair<ChildGadget, QList<int>>") << QVariant::fromValue<QPair<ChildGadget, QList<int>>>({ChildGadget(), {1, 2, 3}})
													<< qMetaTypeId<QPair<QVariant, QVariant>>();

	QTest::newRow("QList<QPair<bool, bool>>") << QVariant::fromValue<QList<QPair<bool, bool>>>({{false, true}, {true, false}})
											  << (int)QVariant::List;

	QTest::newRow("QVariantHash") << QVariant(QVariantHash({{"baum", 42}}))
								  << (int)QVariant::Map;
	QTest::newRow("QVariantMap") << QVariant(QVariantMap({{"baum", 42}}))
								 << (int)QVariant::Hash;
}

void GadgetSerializerTest::testVariantConversions()
{
	QFETCH(QVariant, data);
	QFETCH(int, targetType);

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 2)
	QEXPECT_FAIL("QVariantHash", "not until Qt 5.9.2", Abort);
	QEXPECT_FAIL("QVariantMap", "not until Qt 5.9.2", Abort);
#endif

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

void GadgetSerializerTest::testInvalidDeserialization()
{
	auto broken = TestGadget::createJson({
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
	auto testJson = TestGadget::createJson({
									{"childGadget", QJsonValue::Null}
								});

	try {
		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestGadget>(testJson), QJsonSerializerException);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testNullDeserialization()
{
	auto testJson = QJsonObject({
									{"intProperty", QJsonValue::Null},
									{"boolProperty", QJsonValue::Null},
									{"stringProperty", QJsonValue::Null},
									{"doubleProperty", QJsonValue::Null},
									{"normalEnumProperty", QJsonValue::Null},
									{"enumFlagsProperty", QJsonValue::Null},
									{"datetime", QJsonValue::Null},
									{"uuid", QJsonValue::Null},
									{"url", QJsonValue::Null},
									{"leveledList", QJsonValue::Null},
									{"simpleMap", QJsonValue::Null},
									{"leveledMap", QJsonValue::Null},
									{"childGadget", QJsonValue::Null},
									{"simpleChildren", QJsonValue::Null},
									{"leveledChildren", QJsonValue::Null},
									{"simpleRelatives", QJsonValue::Null},
									{"leveledRelatives", QJsonValue::Null},
									{"object", QJsonValue::Null},
									{"array", QJsonValue::Null},
									{"value", QJsonValue::Null}
								});

	try {
		serializer->setAllowDefaultNull(false);
		QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestGadget>(testJson), QJsonSerializerException);

		serializer->setAllowDefaultNull(true);
		QCOMPARE(serializer->deserialize<TestGadget>(testJson), TestGadget());
	} catch(QException &e) {
		QFAIL(e.what());
	}

	serializer->setAllowDefaultNull(false);
}

void GadgetSerializerTest::testDeserializationValidation_data()
{
	QTest::addColumn<QJsonObject>("data");
	QTest::addColumn<QJsonSerializer::ValidationFlags>("flags");
	QTest::addColumn<bool>("success");

	QTest::newRow("standard") << TestGadget::createJson({
															{"garbage", QJsonValue::Null} //extra property
														},
														"leveledMap")
							  << (QJsonSerializer::ValidationFlags)QJsonSerializer::StandardValidation
							  << true;

	QTest::newRow("validateExtra") << TestGadget::createJson({
													  {"garbage", QJsonValue::Null} //extra property
												  })
								   << (QJsonSerializer::ValidationFlags)QJsonSerializer::NoExtraProperties
								   << false;

	QTest::newRow("validateMissing") << TestGadget::createJson({}, "leveledMap")
									 << (QJsonSerializer::ValidationFlags)QJsonSerializer::AllProperties
									 << false;

	QTest::newRow("validateAll") << TestGadget::createJson({
															   {"garbage", QJsonValue::Null} //extra property
														   },
														   "leveledMap")
								 << (QJsonSerializer::ValidationFlags)QJsonSerializer::FullValidation
								 << false;
}

void GadgetSerializerTest::testDeserializationValidation()
{
	QFETCH(QJsonObject, data);
	QFETCH(QJsonSerializer::ValidationFlags, flags);
	QFETCH(bool, success);

	serializer->setValidationFlags(flags);

	try {
		if(success) {
			auto deser = serializer->deserialize<TestGadget>(data, this);
			QCOMPARE(deser, TestGadget());
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestGadget>(data, this), QJsonDeserializationException);
	} catch (QException &e) {
		QFAIL(e.what());
	}

	serializer->setValidationFlags(QJsonSerializer::StandardValidation);
}

void GadgetSerializerTest::testEnumSpecialSerialization_data()
{
	QTest::addColumn<TestGadget>("gadget");
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<bool>("asString");

	QTest::newRow("normal") << TestGadget::createEnum(TestGadget::Normal1, TestGadget::Flag2)
							<< TestGadget::createJson({
														  {"normalEnumProperty", TestGadget::Normal1},
														  {"enumFlagsProperty", TestGadget::Flag2},
													  })
							<< false;

	QTest::newRow("stringEnum") << TestGadget::createEnum(TestGadget::Normal1, 0)
								<< TestGadget::createJson({
															  {"normalEnumProperty", "Normal1"},
															  {"enumFlagsProperty", ""}
														  })
								<< true;

	QTest::newRow("stringFlags") << TestGadget::createEnum(TestGadget::Normal0, TestGadget::FlagX)
								 << TestGadget::createJson({
															   {"normalEnumProperty", "Normal0"},
															   {"enumFlagsProperty", "FlagX"},
														   })
								 << true;

	QTest::newRow("stringMultiFlags") << TestGadget::createEnum(TestGadget::Normal0, TestGadget::Flag1 | TestGadget::Flag3)
									  << TestGadget::createJson({
																	{"normalEnumProperty", "Normal0"},
																	{"enumFlagsProperty", "Flag1|Flag3"}
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

	serializer->setEnumAsString(false);
}

void GadgetSerializerTest::testEnumSpecialDeserialization_data()
{
	QTest::addColumn<TestGadget>("result");
	QTest::addColumn<QJsonObject>("data");
	QTest::addColumn<bool>("success");

	QTest::newRow("normal") << TestGadget::createEnum(TestGadget::Normal1, 0)
							<< TestGadget::createJson({
														  {"normalEnumProperty", TestGadget::Normal1},
														  {"enumFlagsProperty", 0},
													  })
							<< true;

	QTest::newRow("stringEnum") << TestGadget::createEnum(TestGadget::Normal1, 0)
								<< TestGadget::createJson({
															  {"normalEnumProperty", "Normal1"},
															  {"enumFlagsProperty", QString()},
														  })
								<< true;

	QTest::newRow("invalidEnum") << TestGadget::createEnum(TestGadget::Normal2, 0)
								 << TestGadget::createJson({
															   {"normalEnumProperty", "baum"}
														   })
								 << false;

	QTest::newRow("stringFlags") << TestGadget::createEnum(TestGadget::Normal0, TestGadget::FlagX)
								 << TestGadget::createJson({
															   {"enumFlagsProperty", "FlagX"}
														   })
								 << true;

	QTest::newRow("stringMultiFlags") << TestGadget::createEnum(TestGadget::Normal0, TestGadget::Flag1 | TestGadget::Flag3)
									  << TestGadget::createJson({
																	{"enumFlagsProperty", "Flag1|Flag3"}
																})
									  << true;

	QTest::newRow("invalidFlags") << TestGadget::createEnum(TestGadget::Normal0, TestGadget::FlagX)
								  << TestGadget::createJson({
																{"enumFlagsProperty", "baum"}
															})
								  << false;
}

void GadgetSerializerTest::testEnumSpecialDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(TestGadget, result);
	QFETCH(bool, success);

	try {
		if(success) {
			auto obj = serializer->deserialize<TestGadget>(data, this);
			QCOMPARE(obj, result);
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->deserialize<TestGadget>(data, this), QJsonDeserializationException);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void GadgetSerializerTest::testDeviceSerialization_data()
{
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QByteArray>("fakeDevice");
	QTest::addColumn<bool>("works");

	QTest::newRow("object") << QVariant::fromValue(TestGadget::createBasic(42, true, "baum", 4.2))
							<< QByteArray()
							<< true;

	QTest::newRow("list") << QVariant::fromValue(QList<TestGadget>({TestGadget::createBasic(42, true, "baum", 4.2), TestGadget()}))
						  << QByteArray()
						  << true;

	QTest::newRow("data") << QVariant(42)
						  << QByteArray()
						  << false;

	QTest::newRow("objectdata") << QVariant::fromValue(TestGadget())
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

void GadgetSerializerTest::testExceptionTrace()
{
	try {
		serializer->deserialize<TestGadget>(TestGadget::createJson({
																	   {"childGadget", QJsonObject({
																			{"data", "test"}
																		})}
																   }));
		QFAIL("No exception thrown");
	} catch (QJsonSerializerException &e) {
		auto trace = e.propertyTrace();
		QCOMPARE(trace.size(), 2);
		QCOMPARE(trace[0].first, QByteArray("childGadget"));
		QCOMPARE(trace[0].second, QByteArray("ChildGadget"));
		QCOMPARE(trace[1].first, QByteArray("data"));
		QCOMPARE(trace[1].second, QByteArray("int"));
	}
}

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

	{
		QDate date(1111, 11, 11);
		QTime time(22, 22, 22);
		QDateTime dateTime(date, time);
		QTest::newRow("datetime") << TestGadget::createExtra(dateTime, {}, {})
								  << TestGadget::createJson({
																{"datetime", dateTime.toString(Qt::ISODate)}
															});
	}

	{
		auto uuid = QUuid::createUuid();
		QTest::newRow("uuid") << TestGadget::createExtra({}, uuid, {})
							  << TestGadget::createJson({
															{"uuid", uuid.toString()}
														});
	}

	{
		QUrl url(QStringLiteral("https://user:password@login.example.com:4711/in/some/file.txt?b=42&z=0#baum42"));
		QTest::newRow("uuid") << TestGadget::createExtra({}, {}, url)
							  << TestGadget::createJson({
															{"url", url.toString()}
														});
	}

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

	QTest::newRow("pair") << TestGadget::createPair({42, "baum"}, {}, {})
						  << TestGadget::createJson({
														{"pair", QJsonArray({42, "baum"})}
													});
	QTest::newRow("extraPair") << TestGadget::createPair({}, {ChildGadget(42), {6, 6, 666}}, {})
							   << TestGadget::createJson({
															 {"extraPair", QJsonArray({ChildGadget::createJson(42), QJsonArray({6, 6, 666})})}
														 });
	QTest::newRow("listPair") << TestGadget::createPair({}, {}, {{true, false}, {false, true}})
							  << TestGadget::createJson({
															{"listPair", QJsonArray({QJsonArray({true, false}), QJsonArray({false, true})})}
														});

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

	QTest::newRow("jobject") << TestGadget::createEmbedded(ChildGadget::createJson(), QJsonArray(), QJsonValue())
							 << TestGadget::createJson({
														   {"object", ChildGadget::createJson()}
													   });

	QTest::newRow("jarray") << TestGadget::createEmbedded(QJsonObject(), QJsonArray({1, 2, 3}), QJsonValue())
							<< TestGadget::createJson({
														  {"array", QJsonArray({1, 2, 3})}
													  });

	QTest::newRow("jvalue") << TestGadget::createEmbedded(QJsonObject(), QJsonArray(), 42)
							<< TestGadget::createJson({
														  {"value", 42}
													  });
}

static void compile_test()
{
	QJsonSerializer s;
	QVariant v;
	TestGadget t;
	QList<TestGadget> l;
	QMap<QString, TestGadget> m;
	int i;
	QString str;
	QList<int> il;
	QMap<QString, bool> bm;
	QPair<double, bool> dbp;

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
	jv = s.serialize(i);
	jv = s.serialize(str);
	ja = s.serialize(il);
	jo = s.serialize(bm);
	ja = s.serialize(dbp);

	s.serializeTo(d, v);
	s.serializeTo(d, t);
	s.serializeTo(d, l);
	s.serializeTo(d, m);
	s.serializeTo(d, i);
	s.serializeTo(d, str);
	s.serializeTo(d, il);
	s.serializeTo(d, bm);
	s.serializeTo(d, dbp);

	b = s.serializeTo(v);
	b = s.serializeTo(t);
	b = s.serializeTo(l);
	b = s.serializeTo(m);
	b = s.serializeTo(i);
	b = s.serializeTo(str);
	b = s.serializeTo(il);
	b = s.serializeTo(bm);
	b = s.serializeTo(dbp);

	v = s.deserialize(jv, qMetaTypeId<TestGadget>());
	v = s.deserialize(jv, qMetaTypeId<TestGadget>(), p);
	t = s.deserialize<TestGadget>(jo);
	t = s.deserialize<TestGadget>(jo, p);
	l = s.deserialize<QList<TestGadget>>(ja);
	l = s.deserialize<QList<TestGadget>>(ja, p);
	m = s.deserialize<QMap<QString, TestGadget>>(jo);
	m = s.deserialize<QMap<QString, TestGadget>>(jo, p);
	i = s.deserialize<int>(jv);
	i = s.deserialize<int>(jv, p);
	str = s.deserialize<QString>(jv);
	str = s.deserialize<QString>(jv, p);
	il = s.deserialize<QList<int>>(ja);
	il = s.deserialize<QList<int>>(ja, p);
	bm = s.deserialize<QMap<QString, bool>>(jo);
	bm = s.deserialize<QMap<QString, bool>>(jo, p);
	dbp = s.deserialize<QPair<double, bool>>(ja);
	dbp = s.deserialize<QPair<double, bool>>(ja, p);

	v = s.deserializeFrom(d, qMetaTypeId<TestGadget>());
	v = s.deserializeFrom(d, qMetaTypeId<TestGadget>(), p);
	t = s.deserializeFrom<TestGadget>(d);
	t = s.deserializeFrom<TestGadget>(d, p);
	l = s.deserializeFrom<QList<TestGadget>>(d);
	l = s.deserializeFrom<QList<TestGadget>>(d, p);
	m = s.deserializeFrom<QMap<QString, TestGadget>>(d);
	m = s.deserializeFrom<QMap<QString, TestGadget>>(d, p);
	i = s.deserializeFrom<int>(d);
	i = s.deserializeFrom<int>(d, p);
	str = s.deserializeFrom<QString>(d);
	str = s.deserializeFrom<QString>(d, p);
	il = s.deserializeFrom<QList<int>>(d);
	il = s.deserializeFrom<QList<int>>(d, p);
	bm = s.deserializeFrom<QMap<QString, bool>>(d);
	bm = s.deserializeFrom<QMap<QString, bool>>(d, p);
	dbp = s.deserializeFrom<QPair<double, bool>>(d);
	dbp = s.deserializeFrom<QPair<double, bool>>(d, p);

	v = s.deserializeFrom(b, qMetaTypeId<TestGadget>());
	v = s.deserializeFrom(b, qMetaTypeId<TestGadget>(), p);
	t = s.deserializeFrom<TestGadget>(b);
	t = s.deserializeFrom<TestGadget>(b, p);
	l = s.deserializeFrom<QList<TestGadget>>(b);
	l = s.deserializeFrom<QList<TestGadget>>(b, p);
	m = s.deserializeFrom<QMap<QString, TestGadget>>(b);
	m = s.deserializeFrom<QMap<QString, TestGadget>>(b, p);
	i = s.deserializeFrom<int>(b);
	i = s.deserializeFrom<int>(b, p);
	str = s.deserializeFrom<QString>(b);
	str = s.deserializeFrom<QString>(b, p);
	il = s.deserializeFrom<QList<int>>(b);
	il = s.deserializeFrom<QList<int>>(b, p);
	bm = s.deserializeFrom<QMap<QString, bool>>(b);
	bm = s.deserializeFrom<QMap<QString, bool>>(b, p);
	dbp = s.deserializeFrom<QPair<double, bool>>(b);
	dbp = s.deserializeFrom<QPair<double, bool>>(b, p);
}

QTEST_MAIN(GadgetSerializerTest)

#include "tst_gadgetserializer.moc"
