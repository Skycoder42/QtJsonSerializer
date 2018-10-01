#include <QtTest>
#include <QtJsonSerializer>

#include "testgadget.h"
#include "testobject.h"

using TestTuple = std::tuple<int, QString, QList<int>>;
using TestPair = std::pair<bool, int>;
Q_DECLARE_METATYPE(TestTuple)
Q_DECLARE_METATYPE(TestPair)

class SerializerTest : public QObject
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

private:
	QJsonSerializer *serializer = nullptr;

	void addCommonData();
};

void SerializerTest::initTestCase()
{
	QJsonSerializer::registerListConverters<QList<int>>();
	QJsonSerializer::registerMapConverters<QMap<QString, int>>();

	QJsonSerializer::registerAllConverters<TestGadget>();
	QJsonSerializer::registerListConverters<QList<TestGadget>>();
	QJsonSerializer::registerMapConverters<QMap<QString, TestGadget>>();

	QJsonSerializer::registerAllConverters<TestObject*>();
	QJsonSerializer::registerListConverters<QList<TestObject*>>();
	QJsonSerializer::registerMapConverters<QMap<QString, TestObject*>>();
	QJsonSerializer::registerPointerConverters<TestObject>();

	QJsonSerializer::registerPairConverters<int, QString>();
	QJsonSerializer::registerPairConverters<bool, bool>();
	QJsonSerializer::registerPairConverters<TestGadget, QList<int>>();
	QJsonSerializer::registerPairConverters<TestObject*, QList<int>>();
	QJsonSerializer::registerListConverters<QPair<bool, bool>>();

	QJsonSerializer::registerTupleConverters<int, QString, QList<int>>("std::tuple<int, QString, QList<int>>");
	QJsonSerializer::registerPairConverters<bool, int>("std::pair<bool, int>");

	//register list comparators, needed for test only!
	QMetaType::registerEqualsComparator<QList<int>>();
	QMetaType::registerEqualsComparator<QList<QList<int>>>();
	QMetaType::registerEqualsComparator<QMap<QString, int>>();
	QMetaType::registerEqualsComparator<QMap<QString, QMap<QString, int>>>();

	QMetaType::registerEqualsComparator<TestGadget>();
	QMetaType::registerEqualsComparator<QList<TestGadget>>();
	QMetaType::registerEqualsComparator<QList<QList<TestGadget>>>();
	QMetaType::registerEqualsComparator<QMap<QString, TestGadget>>();
	QMetaType::registerEqualsComparator<QMap<QString, QMap<QString, TestGadget>>>();

	QMetaType::registerEqualsComparator<TestObject*>();
	QMetaType::registerEqualsComparator<QList<TestObject*>>();
	QMetaType::registerEqualsComparator<QList<QList<TestObject*>>>();
	QMetaType::registerEqualsComparator<QMap<QString, TestObject*>>();
	QMetaType::registerEqualsComparator<QMap<QString, QMap<QString, TestObject*>>>();

	QMetaType::registerEqualsComparator<QPair<int, QString>>();
	QMetaType::registerEqualsComparator<QPair<TestGadget, QList<int>>>();
	QMetaType::registerEqualsComparator<QPair<TestObject*, QList<int>>>();
	QMetaType::registerEqualsComparator<QList<QPair<bool, bool>>>();

	QMetaType::registerEqualsComparator<TestTuple>();
	QMetaType::registerEqualsComparator<TestPair>();

	serializer = new QJsonSerializer{this};
}

void SerializerTest::cleanupTestCase()
{
	delete serializer;
	serializer = nullptr;
}

void SerializerTest::testVariantConversions_data()
{
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<int>("targetType");

	QTest::newRow("QList<int>") << QVariant::fromValue<QList<int>>({3, 7, 13})
								<< static_cast<int>(QVariant::List);
	QList<int> l1 = {0, 1, 2};
	QList<int> l2 = {3, 4, 5};
	QList<int> l3 = {6, 7, 8};
	QTest::newRow("QList<QList<int>>") << QVariant::fromValue<QList<QList<int>>>({l1, l2, l3})
									   << static_cast<int>(QVariant::List);
	QTest::newRow("QStringList") << QVariant::fromValue<QStringList>({QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")})
								 << static_cast<int>(QVariant::List);

	QTest::newRow("QMap<QString, int>") << QVariant::fromValue<QMap<QString, int>>({
																					   {QStringLiteral("baum"), 42},
																					   {QStringLiteral("devil"), 666},
																					   {QStringLiteral("fun"), 0}
																				   })
										<< static_cast<int>(QVariant::Map);
	QMap<QString, int> m1 = {{QStringLiteral("v0"), 0}, {QStringLiteral("v1"), 1}, {QStringLiteral("v2"), 2}};
	QMap<QString, int> m2 = {{QStringLiteral("v3"), 3}, {QStringLiteral("v4"), 4}, {QStringLiteral("v5"), 5}};
	QMap<QString, int> m3 = {{QStringLiteral("v6"), 6}, {QStringLiteral("v7"), 7}, {QStringLiteral("v8"), 8}};
	QTest::newRow("QMap<QString, QMap<QString, int>>") << QVariant::fromValue<QMap<QString, QMap<QString, int>>>({
																													 {QStringLiteral("m1"), m1},
																													 {QStringLiteral("m2"), m2},
																													 {QStringLiteral("m3"), m3}
																												 })
													   << static_cast<int>(QVariant::Map);

	QTest::newRow("QList<TestGadget>") << QVariant::fromValue<QList<TestGadget>>({{}, {}, {}})
									   << static_cast<int>(QVariant::List);
	QList<TestGadget> g1 {{}, {}, {}};
	QList<TestGadget> g2 {{}, {}, {}};
	QList<TestGadget> g3 {{}, {}, {}};
	QTest::newRow("QList<QList<TestGadget>>") << QVariant::fromValue<QList<QList<TestGadget>>>({g1, g2, g3})
											  << static_cast<int>(QVariant::List);

	QTest::newRow("QMap<QString, TestGadget>") << QVariant::fromValue<QMap<QString, TestGadget>>({
																									 {QStringLiteral("baum"), {}},
																									 {QStringLiteral("devil"), {}},
																									 {QStringLiteral("fun"), {}}
																								  })
											   << static_cast<int>(QVariant::Map);
	QMap<QString, TestGadget> r1 = {
		{QStringLiteral("v0"), {}},
		{QStringLiteral("v1"), {}},
		{QStringLiteral("v2"), {}}};
	QMap<QString, TestGadget> r2 = {
		{QStringLiteral("v3"), {}},
		{QStringLiteral("v4"), {}},
		{QStringLiteral("v5"), {}}};
	QMap<QString, TestGadget> r3 = {
		{QStringLiteral("v6"), {}},
		{QStringLiteral("v7"), {}},
		{QStringLiteral("v8"), {}}};
	QTest::newRow("QMap<QString, QMap<QString, TestGadget>>") << QVariant::fromValue<QMap<QString, QMap<QString, TestGadget>>>({
																																   {QStringLiteral("r1"), r1},
																																   {QStringLiteral("r2"), r2},
																																   {QStringLiteral("r3"), r3}
																															   })
															  << static_cast<int>(QVariant::Map);

	QTest::newRow("QList<TestObject*>") << QVariant::fromValue<QList<TestObject*>>({new TestObject{this}, new TestObject{this}, new TestObject{this}})
									   << static_cast<int>(QVariant::List);
	QList<TestObject*> o1 {new TestObject{this}, new TestObject{this}, new TestObject{this}};
	QList<TestObject*> o2 {new TestObject{this}, new TestObject{this}, new TestObject{this}};
	QList<TestObject*> o3 {new TestObject{this}, new TestObject{this}, new TestObject{this}};
	QTest::newRow("QList<QList<TestObject*>>") << QVariant::fromValue<QList<QList<TestObject*>>>({o1, o2, o3})
											  << static_cast<int>(QVariant::List);

	QTest::newRow("QMap<QString, TestObject*>") << QVariant::fromValue<QMap<QString, TestObject*>>({
																									   {QStringLiteral("baum"), new TestObject{this}},
																									   {QStringLiteral("devil"), new TestObject{this}},
																									   {QStringLiteral("fun"), new TestObject{this}}
																								   })
											   << static_cast<int>(QVariant::Map);
	QMap<QString, TestObject*> q1 = {
		{QStringLiteral("v0"), new TestObject{this}},
		{QStringLiteral("v1"), new TestObject{this}},
		{QStringLiteral("v2"), new TestObject{this}}};
	QMap<QString, TestObject*> q2 = {
		{QStringLiteral("v3"), new TestObject{this}},
		{QStringLiteral("v4"), new TestObject{this}},
		{QStringLiteral("v5"), new TestObject{this}}};
	QMap<QString, TestObject*> q3 = {
		{QStringLiteral("v6"), new TestObject{this}},
		{QStringLiteral("v7"), new TestObject{this}},
		{QStringLiteral("v8"), new TestObject{this}}};
	QTest::newRow("QMap<QString, QMap<QString, TestObject*>>") << QVariant::fromValue<QMap<QString, QMap<QString, TestObject*>>>({
																																	 {QStringLiteral("q1"), q1},
																																	 {QStringLiteral("q3"), q2},
																																	 {QStringLiteral("q3"), q3}
																																 })
															  << static_cast<int>(QVariant::Map);

	QTest::newRow("QSharedPointer<TestObject>") << QVariant::fromValue(QSharedPointer<TestObject>(new TestObject(nullptr)))
												<< qMetaTypeId<QSharedPointer<QObject>>();
	QTest::newRow("QPointer<TestObject>") << QVariant::fromValue<QPointer<TestObject>>(new TestObject(this))
										  << qMetaTypeId<QPointer<QObject>>();

	QTest::newRow("QPair<int, QString>") << QVariant::fromValue<QPair<int, QString>>({42, QStringLiteral("baum")})
										 << qMetaTypeId<QPair<QVariant, QVariant>>();

	QTest::newRow("QPair<TestGadget, QList<int>>") << QVariant::fromValue<QPair<TestGadget, QList<int>>>({{}, {1, 2, 3}})
												   << qMetaTypeId<QPair<QVariant, QVariant>>();
	QTest::newRow("QPair<TestObject*, QList<int>>") << QVariant::fromValue<QPair<TestObject*, QList<int>>>({new TestObject{this}, {1, 2, 3}})
													<< qMetaTypeId<QPair<QVariant, QVariant>>();

	QTest::newRow("QList<QPair<bool, bool>>") << QVariant::fromValue<QList<QPair<bool, bool>>>({{false, true}, {true, false}})
											  << static_cast<int>(QVariant::List);

	QTest::newRow("std::tuple<int, QString, QList<int>>") << QVariant::fromValue<TestTuple>(std::make_tuple(42, QStringLiteral("Hello World"), QList<int>{1, 2, 3}))
														  << static_cast<int>(QVariant::List);
	QTest::newRow("std::pair<bool, int>>") << QVariant::fromValue<TestPair>(std::make_pair(true, 20))
										   << qMetaTypeId<QPair<QVariant, QVariant>>();
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

void SerializerTest::testSerialization_data()
{
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QJsonValue>("result");
	QTest::addColumn<bool>("works");

	addCommonData();
}

void SerializerTest::testSerialization()
{
	QFETCH(QVariant, data);
	QFETCH(QJsonValue, result);
	QFETCH(bool, works);

	try {
		if(works) {
			auto res = serializer->serialize(data);
			QCOMPARE(res, result);
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->serialize(data), QJsonSerializationException);
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void SerializerTest::testDeserialization_data()
{
	QTest::addColumn<QVariant>("result");
	QTest::addColumn<QJsonValue>("data");
	QTest::addColumn<bool>("works");

	addCommonData();
}

void SerializerTest::testDeserialization()
{
	QFETCH(QJsonValue, data);
	QFETCH(QVariant, result);
	QFETCH(bool, works);

	try {
		if(works) {
			auto res = serializer->deserialize(data, result.userType(), this);
			QCOMPARE(res, result);
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->deserialize(data, result.userType(), this), QJsonDeserializationException);
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void SerializerTest::addCommonData()
{
	//basic types without any converter
	QTest::newRow("bool") << QVariant{true}
						  << QJsonValue{true}
						  << true;
	QTest::newRow("int") << QVariant{42}
						 << QJsonValue{42}
						 << true;
	QTest::newRow("double") << QVariant{4.2}
							<< QJsonValue{4.2}
							<< true;
	QTest::newRow("string") << QVariant{QStringLiteral("baum")}
							<< QJsonValue{QStringLiteral("baum")}
							<< true;
	QTest::newRow("nullptr") << QVariant::fromValue(nullptr)
							 << QJsonValue{QJsonValue::Null}
							 << true;
}

QTEST_MAIN(SerializerTest)

#include "tst_serializer.moc"
