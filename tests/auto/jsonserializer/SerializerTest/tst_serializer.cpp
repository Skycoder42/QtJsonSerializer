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

	void testDeviceSerialization();
	void testExceptionTrace();

private:
	QJsonSerializer *serializer = nullptr;

	void addCommonData();
	void resetProps();
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
	QTest::addColumn<QVariantHash>("extraProps");

	addCommonData();
}

void SerializerTest::testSerialization()
{
	QFETCH(QVariant, data);
	QFETCH(QJsonValue, result);
	QFETCH(bool, works);
	QFETCH(QVariantHash, extraProps);

	resetProps();
	for(auto it = extraProps.constBegin(); it != extraProps.constEnd(); it++)
		serializer->setProperty(qUtf8Printable(it.key()), it.value());

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
	QTest::addColumn<QVariantHash>("extraProps");

	addCommonData();

	QTest::newRow("null.invalid.bool") << QVariant{false}
									   << QJsonValue{QJsonValue::Null}
									   << false
									   << QVariantHash{};
	QTest::newRow("null.invalid.int") << QVariant{0}
									  << QJsonValue{QJsonValue::Null}
									  << false
									  << QVariantHash{};
	QTest::newRow("null.invalid.double") << QVariant{0.0}
										 << QJsonValue{QJsonValue::Null}
										 << false
										 << QVariantHash{};
	QTest::newRow("null.invalid.string") << QVariant{QString()}
										 << QJsonValue{QJsonValue::Null}
										 << false
										 << QVariantHash{};
	QTest::newRow("null.invalid.gadget") << QVariant::fromValue<TestGadget>({})
										 << QJsonValue{QJsonValue::Null}
										 << false
										 << QVariantHash{};

	QTest::newRow("null.valid.bool") << QVariant{false}
									 << QJsonValue{QJsonValue::Null}
									 << true
									 << QVariantHash{{QStringLiteral("allowDefaultNull"), true}};
	QTest::newRow("null.valid.int") << QVariant{0}
									<< QJsonValue{QJsonValue::Null}
									<< true
									<< QVariantHash{{QStringLiteral("allowDefaultNull"), true}};
	QTest::newRow("null.valid.double") << QVariant{0.0}
									   << QJsonValue{QJsonValue::Null}
									   << true
									   << QVariantHash{{QStringLiteral("allowDefaultNull"), true}};
	QTest::newRow("null.valid.string") << QVariant{QString()}
									   << QJsonValue{QJsonValue::Null}
									   << true
									   << QVariantHash{{QStringLiteral("allowDefaultNull"), true}};
	QTest::newRow("null.valid.gadget") << QVariant::fromValue<TestGadget>({})
									   << QJsonValue{QJsonValue::Null}
									   << true
									   << QVariantHash{{QStringLiteral("allowDefaultNull"), true}};
}

void SerializerTest::testDeserialization()
{
	QFETCH(QJsonValue, data);
	QFETCH(QVariant, result);
	QFETCH(bool, works);
	QFETCH(QVariantHash, extraProps);

	resetProps();
	for(auto it = extraProps.constBegin(); it != extraProps.constEnd(); it++)
		serializer->setProperty(qUtf8Printable(it.key()), it.value());

	try {
		if(works) {
			auto res = serializer->deserialize(data, result.userType(), this);
			if(result.userType() == qMetaTypeId<TestObject*>())
				QVERIFY(TestObject::equals(res.value<TestObject*>(), result.value<TestObject*>()));
			else
				QCOMPARE(res, result);
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->deserialize(data, result.userType(), this), QJsonDeserializationException);
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void SerializerTest::testDeviceSerialization()
{
	const TestGadget g{10};
	const QByteArray bRes{R"__({"data":10})__"};

	// to bytearray
	auto ba = serializer->serializeTo(g, QJsonDocument::Compact);
	QCOMPARE(ba, bRes);
	auto gRes = serializer->deserializeFrom<TestGadget>(ba);
	QCOMPARE(gRes, g);

	//to device
	ba.clear();
	QBuffer buffer{&ba};
	QVERIFY(buffer.open(QIODevice::WriteOnly | QIODevice::Text));
	serializer->serializeTo(&buffer, g, QJsonDocument::Compact);
	buffer.close();
	QCOMPARE(ba, bRes);
	QVERIFY(buffer.open(QIODevice::ReadOnly | QIODevice::Text));
	gRes = serializer->deserializeFrom<TestGadget>(&buffer);
	buffer.close();
	QCOMPARE(gRes, g);

	//invalid
	QVERIFY_EXCEPTION_THROWN(serializer->serializeTo(42), QJsonSerializationException);
}

void SerializerTest::testExceptionTrace()
{
	try {
		serializer->deserialize<QList<TestGadget>>({
													   QJsonObject{
														   {QStringLiteral("data"), QStringLiteral("test")}
													   }
												   });
		QFAIL("No exception thrown");
	} catch (QJsonSerializerException &e) {
		auto trace = e.propertyTrace();
		QCOMPARE(trace.size(), 2);
		QCOMPARE(trace[0].first, QByteArray{"[0]"});
		QCOMPARE(trace[0].second, QByteArray{"TestGadget"});
		QCOMPARE(trace[1].first, QByteArray{"data"});
		QCOMPARE(trace[1].second, QByteArray{"int"});
	}
}

void SerializerTest::addCommonData()
{
	//basic types without any converter
	QTest::newRow("bool") << QVariant{true}
						  << QJsonValue{true}
						  << true
						  << QVariantHash{};
	QTest::newRow("int") << QVariant{42}
						 << QJsonValue{42}
						 << true
						 << QVariantHash{};
	QTest::newRow("double") << QVariant{4.2}
							<< QJsonValue{4.2}
							<< true
							<< QVariantHash{};
	QTest::newRow("string.normal") << QVariant{QStringLiteral("baum")}
								   << QJsonValue{QStringLiteral("baum")}
								   << true
								   << QVariantHash{};
	QTest::newRow("string.empty") << QVariant{QString()}
								  << QJsonValue{QString()}
								  << true
								  << QVariantHash{};
	QTest::newRow("nullptr") << QVariant::fromValue(nullptr)
							 << QJsonValue{QJsonValue::Null}
							 << true
							 << QVariantHash{};

	//advanced types
	QTest::newRow("date.valid") << QVariant{QDate{2010, 10, 20}}
								<< QJsonValue{QStringLiteral("2010-10-20")}
								<< true
								<< QVariantHash{};
	QTest::newRow("date.invalid") << QVariant{QDate{}}
								  << QJsonValue{QString{}}
								  << true
								  << QVariantHash{};
	QTest::newRow("time.valid") << QVariant{QTime{14, 30, 15, 123}}
								<< QJsonValue{QStringLiteral("14:30:15.123")}
								<< true
								<< QVariantHash{};
	QTest::newRow("time.invalid") << QVariant{QTime{}}
								  << QJsonValue{QString{}}
								  << true
								  << QVariantHash{};
	QTest::newRow("datetime.valid") << QVariant{QDateTime{QDate{2010, 10, 20}, QTime{14, 30}}}
									<< QJsonValue{QStringLiteral("2010-10-20T14:30:00.000")}
									<< true
									<< QVariantHash{};
	QTest::newRow("datetime.invalid") << QVariant{QDateTime{}}
									  << QJsonValue{QString{}}
									  << true
									  << QVariantHash{};
	auto id = QUuid::createUuid();
	QTest::newRow("uuid") << QVariant{id}
						  << QJsonValue{id.toString(QUuid::WithoutBraces)}
						  << true
						  << QVariantHash{};
	QTest::newRow("url.valid") << QVariant{QUrl{QStringLiteral("https://example.com/test.xml?baum=42#tree")}}
							   << QJsonValue{QStringLiteral("https://example.com/test.xml?baum=42#tree")}
							   << true
							   << QVariantHash{};
	QTest::newRow("url.invalid") << QVariant{QUrl{}}
								 << QJsonValue{QString{}}
								 << true
								 << QVariantHash{};

	//objects/gadget -> ser-helper tests
	QTest::newRow("gadget.normal") << QVariant::fromValue<TestGadget>(42)
								   << QJsonValue{QJsonObject{{QStringLiteral("data"), 42}}}
								   << true
								   << QVariantHash{};
	QTest::newRow("object.normal") << QVariant::fromValue(new TestObject{24})
								   << QJsonValue{QJsonObject{{QStringLiteral("data"), 24}}}
								   << true
								   << QVariantHash{};
	QTest::newRow("object.null") << QVariant::fromValue<TestObject*>(nullptr)
								 << QJsonValue{QJsonValue::Null}
								 << true
								 << QVariantHash{};
	QTest::newRow("object.null") << QVariant::fromValue<TestObject*>(nullptr)
								 << QJsonValue{QJsonValue::Null}
								 << true
								 << QVariantHash{};
	QTest::newRow("object.name") << QVariant::fromValue(new TestObject{10})
								 << QJsonValue{QJsonObject{{QStringLiteral("data"), 10}, {QStringLiteral("objectName"), QStringLiteral("testname")}}}
								 << true
								 << QVariantHash{{QStringLiteral("keepObjectName"), true}};

	// simple list -> more ser-helper tests
	QTest::newRow("list.empty") << QVariant::fromValue<QList<int>>({})
								<< QJsonValue{QJsonArray{}}
								<< true
								<< QVariantHash{};
	QTest::newRow("list.int") << QVariant::fromValue<QList<int>>({1, 2, 3})
							  << QJsonValue{QJsonArray{1, 2, 3}}
							  << true
							  << QVariantHash{};

	//enums/flags
	QTest::newRow("enum.int") << QVariant::fromValue<EnumGadget>(EnumGadget::Normal1)
							  << QJsonValue{QJsonObject{
										{QStringLiteral("enumProp"), static_cast<int>(EnumGadget::Normal1)},
										{QStringLiteral("flagsProp"), 0}
									}}
							  << true
							  << QVariantHash{};
	QTest::newRow("enum.string") << QVariant::fromValue<EnumGadget>(EnumGadget::Normal2)
								 << QJsonValue{QJsonObject{
										   {QStringLiteral("enumProp"), QStringLiteral("Normal2")},
										   {QStringLiteral("flagsProp"), QString()}
									   }}
								 << true
								 << QVariantHash{{QStringLiteral("enumAsString"), true}};
	QTest::newRow("flags.int") << QVariant::fromValue<EnumGadget>(EnumGadget::FlagX)
							   << QJsonValue{QJsonObject{
										 {QStringLiteral("enumProp"), static_cast<int>(EnumGadget::Normal0)},
										 {QStringLiteral("flagsProp"), static_cast<int>(EnumGadget::FlagX)}
									 }}
							   << true
							   << QVariantHash{};
	QTest::newRow("flags.string.single") << QVariant::fromValue<EnumGadget>(EnumGadget::Flag1 | EnumGadget::Flag2)
										 << QJsonValue{QJsonObject{
													{QStringLiteral("enumProp"), QStringLiteral("Normal0")},
													{QStringLiteral("flagsProp"), QStringLiteral("FlagX")}
											   }}
										 << true
										 << QVariantHash{{QStringLiteral("enumAsString"), true}};
	QTest::newRow("flags.string.multi") << QVariant::fromValue<EnumGadget>(EnumGadget::Flag1 | EnumGadget::Flag3)
										<< QJsonValue{QJsonObject{
												   {QStringLiteral("enumProp"), QStringLiteral("Normal0")},
												   {QStringLiteral("flagsProp"), QStringLiteral("Flag1|Flag3")}
											  }}
										<< true
										<< QVariantHash{{QStringLiteral("enumAsString"), true}};
}

void SerializerTest::resetProps()
{
	serializer->setAllowDefaultNull(false);
	serializer->setKeepObjectName(false);
	serializer->setEnumAsString(false);
	serializer->setValidateBase64(true);
	serializer->setUseBcp47Locale(true);
	serializer->setValidationFlags(QJsonSerializer::StandardValidation);
	serializer->setPolymorphing(QJsonSerializer::Enabled);
}

namespace  {

template<typename Type, typename Json>
static void test_type() {
	QJsonSerializer s;
	Type v;
	Json j;
	static_assert(std::is_same<Json, decltype(s.serialize(v))>::value, "Wrong value returned by expression");
	s.serializeTo(nullptr, v);
	s.serializeTo(v);
	s.deserialize(QJsonValue{}, qMetaTypeId<Type>());
	s.deserialize(QJsonValue{}, qMetaTypeId<Type>(), nullptr);
	s.deserialize<Type>(j);
	s.deserialize<Type>(j, nullptr);
	s.deserializeFrom(nullptr, qMetaTypeId<Type>());
	s.deserializeFrom(nullptr, qMetaTypeId<Type>(), nullptr);
	s.deserializeFrom(QByteArray{}, qMetaTypeId<Type>());
	s.deserializeFrom(QByteArray{}, qMetaTypeId<Type>(), nullptr);
	s.deserializeFrom<Type>(nullptr);
	s.deserializeFrom<Type>(nullptr, nullptr);
	s.deserializeFrom<Type>(QByteArray{});
	s.deserializeFrom<Type>(QByteArray{}, nullptr);
}

Q_DECL_UNUSED void static_compile_test()
{
	test_type<QVariant, QJsonValue>();
	test_type<bool, QJsonValue>();
	test_type<int, QJsonValue>();
	test_type<double, QJsonValue>();
	test_type<QString, QJsonValue>();
	test_type<TestGadget, QJsonObject>();
	test_type<TestGadget*, QJsonObject>();
	test_type<QList<TestGadget>, QJsonArray>();
	test_type<TestObject*, QJsonObject>();
	test_type<QPointer<TestObject>, QJsonObject>();
	test_type<QSharedPointer<TestObject>, QJsonObject>();
	test_type<QList<TestObject*>, QJsonArray>();
	test_type<QMap<QString, TestGadget>, QJsonObject>();
	test_type<QMap<QString, TestObject*>, QJsonObject>();
	test_type<int, QJsonValue>();
	test_type<QString, QJsonValue>();
	test_type<QList<int>, QJsonArray>();
	test_type<QMap<QString, bool>, QJsonObject>();
	test_type<QPair<double, bool>, QJsonArray>();
	test_type<TestTuple, QJsonArray>();
	test_type<TestPair, QJsonArray>();
}

}

QTEST_MAIN(SerializerTest)

#include "tst_serializer.moc"
