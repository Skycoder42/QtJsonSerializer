#include <QtTest>
#include <QtJsonSerializer>

#include "dummyserializationhelper.h"
#include "opaquedummy.h"
#include "testgadget.h"
#include "testobject.h"

// basic converters
#include <QtJsonSerializer/private/qjsonbytearrayconverter_p.h>
#include <QtJsonSerializer/private/qjsongeomconverter_p.h>
#include <QtJsonSerializer/private/qjsonjsonconverter_p.h>
#include <QtJsonSerializer/private/qjsonlocaleconverter_p.h>
#include <QtJsonSerializer/private/qjsonregularexpressionconverter_p.h>
#include <QtJsonSerializer/private/qjsonversionnumberconverter_p.h>

// container converters
#include <QtJsonSerializer/private/qjsonlistconverter_p.h>
#include <QtJsonSerializer/private/qjsonmapconverter_p.h>
#include <QtJsonSerializer/private/qjsonpairconverter_p.h>
#include <QtJsonSerializer/private/qjsonstdtupleconverter_p.h>

// "Object" converter
#include <QtJsonSerializer/private/qjsongadgetconverter_p.h>
#include <QtJsonSerializer/private/qjsonobjectconverter_p.h>

Q_DECLARE_METATYPE(QSharedPointer<QJsonTypeConverter>)
Q_DECLARE_METATYPE(QJsonValue::Type)

template <typename T>
bool operator <(const QMap<QString, T> &m1, const QMap<QString, T> &m2)
{
	return m1.keys() < m2.keys() &&
			m1.values() < m2.values();
}

using TestTpl1 = std::tuple<int, bool, double>;
using TestTpl2 = std::tuple<QList<int>, QPair<bool, bool>, QMap<QString, double>>;
using TestTpl3 = std::tuple<OpaqueDummy>;

Q_DECLARE_METATYPE(TestTpl1)
Q_DECLARE_METATYPE(TestTpl2)
Q_DECLARE_METATYPE(TestTpl3)

class TypeConverterTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testConverterMeta_data();
	void testConverterMeta();
	void testMetaTypeDetection_data();
	void testMetaTypeDetection();

	void testSerialization_data();
	void testSerialization();
	void testDeserialization_data();
	void testDeserialization();

private:
	using TestQ = QList<DummySerializationHelper::SerInfo>;

	DummySerializationHelper *helper;

	QSharedPointer<QJsonTypeConverter> byteConverter;
	QSharedPointer<QJsonTypeConverter> sizeConverter;
	QSharedPointer<QJsonTypeConverter> pointConverter;
	QSharedPointer<QJsonTypeConverter> lineConverter;
	QSharedPointer<QJsonTypeConverter> rectConverter;
	QSharedPointer<QJsonTypeConverter> jValConverter;
	QSharedPointer<QJsonTypeConverter> jObjConverter;
	QSharedPointer<QJsonTypeConverter> jArrConverter;
	QSharedPointer<QJsonTypeConverter> localeConverter;
	QSharedPointer<QJsonTypeConverter> regexConverter;
	QSharedPointer<QJsonTypeConverter> versionConverter;

	QSharedPointer<QJsonTypeConverter> listConverter;
	QSharedPointer<QJsonTypeConverter> mapConverter;
	QSharedPointer<QJsonTypeConverter> pairConverter;
	QSharedPointer<QJsonTypeConverter> tupleConverter;

	QSharedPointer<QJsonTypeConverter> gadgetConverter;
	QSharedPointer<QJsonTypeConverter> objectConverter;

	void addCommonSerData();
};

void TypeConverterTest::initTestCase()
{
	// meta types
	qRegisterMetaType<StaticPolyObject*>();

	//converters
	QJsonSerializer::registerPairConverters<bool, int>();
	QJsonSerializer::registerPairConverters<int, QList<int>>();
	QJsonSerializer::registerTupleConverters<int, bool, double>("std::tuple<int, bool, double>");
	QJsonSerializer::registerTupleConverters<QList<int>, QPair<bool, bool>, QMap<QString, double>>("std::tuple<QList<int>, QPair<bool, bool>, QMap<QString, double>>");
	QJsonSerializer::registerPointerConverters<TestObject>();

	//comparators
	QMetaType::registerEqualsComparator<QVersionNumber>();
	QMetaType::registerEqualsComparator<QList<int>>();
	QMetaType::registerEqualsComparator<QMap<QString, int>>();
	QMetaType::registerEqualsComparator<QPair<bool, int>>();
	QMetaType::registerEqualsComparator<std::pair<bool, int>>();
	QMetaType::registerEqualsComparator<QPair<int, QList<int>>>();
	QMetaType::registerEqualsComparator<TestTpl1>();
	QMetaType::registerEqualsComparator<TestGadget>();

	// helper classes
	helper = new DummySerializationHelper{this};

	byteConverter.reset(new QJsonBytearrayConverter{});
	sizeConverter.reset(new QJsonSizeConverter{});
	pointConverter.reset(new QJsonPointConverter{});
	lineConverter.reset(new QJsonLineConverter{});
	rectConverter.reset(new QJsonRectConverter{});
	jValConverter.reset(new QJsonJsonValueConverter{});
	jObjConverter.reset(new QJsonJsonObjectConverter{});
	jArrConverter.reset(new QJsonJsonArrayConverter{});
	localeConverter.reset(new QJsonLocaleConverter{});
	regexConverter.reset(new QJsonRegularExpressionConverter{});
	versionConverter.reset(new QJsonVersionNumberConverter{});

	listConverter.reset(new QJsonListConverter{});
	mapConverter.reset(new QJsonMapConverter{});
	pairConverter.reset(new QJsonPairConverter{});
	tupleConverter.reset(new QJsonStdTupleConverter{});

	gadgetConverter.reset(new QJsonGadgetConverter{});
	objectConverter.reset(new QJsonObjectConverter{});
}

void TypeConverterTest::cleanupTestCase()
{
	delete helper;
}

void TypeConverterTest::testConverterMeta_data()
{
	QTest::addColumn<QSharedPointer<QJsonTypeConverter>>("converter");
	QTest::addColumn<int>("priority");
	QTest::addColumn<QList<QJsonValue::Type>>("jsonTypes");

	QTest::newRow("regex") << regexConverter
						   << static_cast<int>(QJsonTypeConverter::Standard)
						   << QList<QJsonValue::Type>{QJsonValue::Object, QJsonValue::String};

	QTest::newRow("version") << versionConverter
							 << static_cast<int>(QJsonTypeConverter::Standard)
							 << QList<QJsonValue::Type>{QJsonValue::String};

	QTest::newRow("pair") << pairConverter
						  << static_cast<int>(QJsonTypeConverter::Standard)
						  << QList<QJsonValue::Type>{QJsonValue::Array};

	QTest::newRow("tuple") << tupleConverter
						   << static_cast<int>(QJsonTypeConverter::Standard)
						   << QList<QJsonValue::Type>{QJsonValue::Array};

	QTest::newRow("object") << objectConverter
							<< static_cast<int>(QJsonTypeConverter::Standard)
							<< QList<QJsonValue::Type>{QJsonValue::Object, QJsonValue::Null};
}

void TypeConverterTest::testConverterMeta()
{
	QFETCH(QSharedPointer<QJsonTypeConverter>, converter);
	QFETCH(int, priority);
	QFETCH(QList<QJsonValue::Type>, jsonTypes);

	QCOMPARE(converter->priority(), priority);
	QCOMPARE(converter->jsonTypes(), jsonTypes);
}

void TypeConverterTest::testMetaTypeDetection_data()
{
	QTest::addColumn<QSharedPointer<QJsonTypeConverter>>("converter");
	QTest::addColumn<int>("metatype");
	QTest::addColumn<bool>("matches");

	QTest::newRow("regex.regex") << regexConverter
								 << static_cast<int>(QMetaType::QRegularExpression)
								 << true;
	QTest::newRow("regex.invalid") << regexConverter
								   << static_cast<int>(QMetaType::QString)
								   << false;

	QTest::newRow("version.version") << versionConverter
									 << qMetaTypeId<QVersionNumber>()
									 << true;
	QTest::newRow("version.invalid") << versionConverter
									 << static_cast<int>(QMetaType::QString)
									 << false;

	QTest::newRow("qt.basic") << qMetaTypeId<QPair<QString, int>>()
							  << true;
	QTest::newRow("qt.double") << qMetaTypeId<QPair<int, int>>()
							   << true;
	QTest::newRow("pair.qt.list") << pairConverter
								  << qMetaTypeId<QPair<QList<int>, QList<int>>>()
								  << true;
	QTest::newRow("pair.qt.pair") << pairConverter
								  << qMetaTypeId<QPair<QPair<int, int>, QPair<bool, bool>>>()
								  << true;
	QTest::newRow("pair.qt.map") << pairConverter
								 << qMetaTypeId<QPair<QMap<QString, int>, QList<int>>>()
								 << true;
	QTest::newRow("pair.std.basic") << pairConverter
									<< qMetaTypeId<std::pair<QString, int>>()
									<< true;
	QTest::newRow("pair.std.double") << pairConverter
									 << qMetaTypeId<std::pair<int, int>>()
									 << true;
	QTest::newRow("pair.std.list") << pairConverter
								   << qMetaTypeId<std::pair<QList<int>, QList<int>>>()
								   << true;
	QTest::newRow("pair.std.pair") << pairConverter
								   << qMetaTypeId<std::pair<std::pair<int, int>, std::pair<bool, bool>>>()
								   << true;
	QTest::newRow("pair.std.map") << pairConverter
								  << qMetaTypeId<std::pair<QMap<QString, int>, QList<int>>>()
								  << true;
	QTest::newRow("pair.invalid") << pairConverter
								  << static_cast<int>(QMetaType::QVariantList)
								  << false;

	QTest::newRow("tuple.basic") << tupleConverter
								 << qMetaTypeId<std::tuple<int, bool, double>>()
								 << true;
	QTest::newRow("tuple.extended") << tupleConverter
									<< qMetaTypeId<std::tuple<QList<int>, QPair<bool, bool>, QMap<QString, double>>>()
									<< true;
	QTest::newRow("tuple.invalid") << tupleConverter
								   << static_cast<int>(QMetaType::QVariantList)
								   << false;

	QTest::newRow("object.basic") << objectConverter
								  << qMetaTypeId<TestObject*>()
								  << true;
	QTest::newRow("object.base") << objectConverter
								 << static_cast<int>(QMetaType::QObjectStar)
								 << true;
	QTest::newRow("object.tracking") << objectConverter
									 << qMetaTypeId<QPointer<TestObject>>()
									 << true;
	QTest::newRow("object.shared") << objectConverter
								   << qMetaTypeId<QSharedPointer<TestObject>>()
								   << true;
	QTest::newRow("object.invalid1") << objectConverter
									 << qMetaTypeId<TestGadget*>()
									 << false;
	QTest::newRow("object.invalid2") << objectConverter
									 << qMetaTypeId<TestGadget>()
									 << false;
}

void TypeConverterTest::testMetaTypeDetection()
{
	QFETCH(QSharedPointer<QJsonTypeConverter>, converter);
	QFETCH(int, metatype);
	QFETCH(bool, matches);

	QCOMPARE(converter->canConvert(metatype), matches);
}

void TypeConverterTest::testSerialization_data()
{
	QTest::addColumn<QSharedPointer<QJsonTypeConverter>>("converter");
	QTest::addColumn<QVariantHash>("properties");
	QTest::addColumn<QList<DummySerializationHelper::SerInfo>>("serData");
	QTest::addColumn<QObject*>("parent");
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QJsonValue>("result");

	addCommonSerData();

	QTest::newRow("pair.qt.unconvertible") << pairConverter
										   << QVariantHash{}
										   << TestQ{}
										   << static_cast<QObject*>(nullptr)
										   << qMetaTypeId<QPair<int, OpaqueDummy>>()
										   << QVariant::fromValue(QPair<int, OpaqueDummy>{})
										   << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("pair.std.unconvertible") << pairConverter
											<< QVariantHash{}
											<< TestQ{}
											<< static_cast<QObject*>(nullptr)
											<< qMetaTypeId<std::pair<int, OpaqueDummy>>()
											<< QVariant::fromValue(std::pair<int, OpaqueDummy>{})
											<< QJsonValue{QJsonValue::Undefined};

	QTest::newRow("tuple.unconvertible") << pairConverter
										 << QVariantHash{}
										 << TestQ{}
										 << static_cast<QObject*>(nullptr)
										 << qMetaTypeId<std::tuple<OpaqueDummy>>()
										 << QVariant::fromValue(std::tuple<OpaqueDummy>{})
										 << QJsonValue{QJsonValue::Undefined};
}

void TypeConverterTest::testSerialization()
{
	QFETCH(QSharedPointer<QJsonTypeConverter>, converter);
	QFETCH(QVariantHash, properties);
	QFETCH(QList<DummySerializationHelper::SerInfo>, serData);
	QFETCH(int, type);
	QFETCH(QVariant, data);
	QFETCH(QJsonValue, result);

	helper->properties = properties;
	helper->serData = serData;

	try {
		if(result.isUndefined())
			QVERIFY_EXCEPTION_THROWN(converter->serialize(type, data, helper), QJsonSerializationException);
		else {
			auto res = converter->serialize(type, data, helper);
			QCOMPARE(res, result);
		}
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void TypeConverterTest::testDeserialization_data()
{
	QTest::addColumn<QSharedPointer<QJsonTypeConverter>>("converter");
	QTest::addColumn<QVariantHash>("properties");
	QTest::addColumn<QList<DummySerializationHelper::SerInfo>>("deserData");
	QTest::addColumn<QObject*>("parent");
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("result");
	QTest::addColumn<QJsonValue>("data");

	addCommonSerData();

	QTest::newRow("regex.string") << regexConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QRegularExpression)
								  << QVariant{QRegularExpression{QStringLiteral("just\\sa\\sstring")}}
								  << QJsonValue{QStringLiteral("just\\sa\\sstring")};

	QTest::newRow("version.suffixed") << versionConverter
									  << QVariantHash{}
									  << TestQ{}
									  << static_cast<QObject*>(nullptr)
									  << qMetaTypeId<QVersionNumber>()
									  << QVariant::fromValue(QVersionNumber{1, 2, 3})
									  << QJsonValue{QStringLiteral("1.2.3-r1")};
	QTest::newRow("version.invalid") << versionConverter
									 << QVariantHash{}
									 << TestQ{}
									 << static_cast<QObject*>(nullptr)
									 << qMetaTypeId<QVersionNumber>()
									 << QVariant{}
									 << QJsonValue{QStringLiteral("A1.4.5")};

	QTest::newRow("pair.invalid") << pairConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << qMetaTypeId<QPair<bool, int>>()
								  << QVariant{}
								  << QJsonValue{QJsonArray{true, 2, 3}};

	QTest::newRow("tuple.invalid") << tupleConverter
								   << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<TestTpl1>()
								   << QVariant{}
								   << QJsonValue{QJsonArray{1, true, 3.4, 4}};
}

void TypeConverterTest::testDeserialization()
{
	QFETCH(QSharedPointer<QJsonTypeConverter>, converter);
	QFETCH(QVariantHash, properties);
	QFETCH(QList<DummySerializationHelper::SerInfo>, deserData);
	QFETCH(QObject*, parent);
	QFETCH(int, type);
	QFETCH(QJsonValue, data);
	QFETCH(QVariant, result);

	helper->properties = properties;
	helper->deserData = deserData;
	helper->expectedParent = parent;

	try {
		if(!result.isValid())
			QVERIFY_EXCEPTION_THROWN(converter->deserialize(type, data, this, helper), QJsonDeserializationException);
		else {
			auto res = converter->deserialize(type, data, this, helper);
			QVERIFY(res.convert(type));

			const auto flags = QMetaType::typeFlags(type);
			if(flags.testFlag(QMetaType::PointerToQObject) ||
			   flags.testFlag(QMetaType::TrackingPointerToQObject) ||
			   flags.testFlag(QMetaType::SharedPointerToQObject)) {
				const auto obj1 = res.value<TestObject*>();
				const auto obj2 = result.value<TestObject*>();
				if(obj1 != obj2) { //same object is automatically equal
					QVERIFY(obj1);
					QVERIFY(obj2);
					QVERIFY(obj1->compare(obj2));
				}
			} else if(flags.testFlag(QMetaType::PointerToGadget)) {
				const auto ptr1 = reinterpret_cast<const TestGadget* const *>(res.constData());
				const auto ptr2 = reinterpret_cast<const TestGadget* const *>(result.constData());
				if(ptr1 != ptr2) { //same object is automatically equal
					QVERIFY(ptr1);
					QVERIFY(ptr2);
					const auto gadPtr1 = *ptr1;
					const auto gadPtr2 = *ptr2;
					if(gadPtr1 != gadPtr2) { //same object is automatically equal
						QVERIFY(gadPtr1);
						QVERIFY(gadPtr2);
						const auto &gad1 = *gadPtr1;
						const auto &gad2 = *gadPtr2;
						QCOMPARE(gad1, gad2);
					}
				}
			} else
				QCOMPARE(res, result);
		}
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void TypeConverterTest::addCommonSerData()
{
	QTest::newRow("regex.simple") << regexConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QRegularExpression)
								  << QVariant{QRegularExpression{QStringLiteral("^\\w*m$")}}
								  << QJsonValue{QJsonObject{
											{QStringLiteral("pattern"), QStringLiteral("^\\w*m$")},
											{QStringLiteral("options"), static_cast<int>(QRegularExpression::NoPatternOption)}
										}};
	QTest::newRow("regex.opts") << regexConverter
								<< QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QRegularExpression)
								<< QVariant{QRegularExpression{QStringLiteral("^\\w*m$"), QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::CaseInsensitiveOption}}
								<< QJsonValue{QJsonObject{
										  {QStringLiteral("pattern"), QStringLiteral("^\\w*m$")},
										  {QStringLiteral("options"), static_cast<int>(QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::CaseInsensitiveOption)}
									  }};

	QTest::newRow("version.simple") << versionConverter
									<< QVariantHash{}
									<< TestQ{}
									<< static_cast<QObject*>(nullptr)
									<< qMetaTypeId<QVersionNumber>()
									<< QVariant::fromValue(QVersionNumber{1, 2, 3})
									<< QJsonValue{QStringLiteral("1.2.3")};
	QTest::newRow("version.long") << versionConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << qMetaTypeId<QVersionNumber>()
								  << QVariant::fromValue(QVersionNumber{1, 2, 3, 4, 5})
								  << QJsonValue{QStringLiteral("1.2.3.4.5")};

	QTest::newRow("pair.qt.basic") << pairConverter
								   << QVariantHash{}
								   << TestQ{{QMetaType::Bool, true, 42}, {QMetaType::Int, 42, true}}
								   << static_cast<QObject*>(this)
								   << qMetaTypeId<QPair<bool, int>>()
								   << QVariant::fromValue(QPair<bool, int>{true, 42})
								   << QJsonValue{QJsonArray{42, true}};
	QTest::newRow("pair.qt.advanced") << pairConverter
									  << QVariantHash{}
									  << TestQ{
												{QMetaType::Int, 3, 1},
												{qMetaTypeId<QList<int>>(), QVariant::fromValue(QList<int>{}), 2}
											}
									  << static_cast<QObject*>(this)
									  << qMetaTypeId<QPair<int, QList<int>>>()
									  << QVariant::fromValue(QPair<int, QList<int>>{3, {}})
									  << QJsonValue{QJsonArray{1, 2}};
	QTest::newRow("pair.std") << pairConverter
							  << QVariantHash{}
							  << TestQ{{QMetaType::Bool, true, 42}, {QMetaType::Int, 42, true}}
							  << static_cast<QObject*>(this)
							  << qMetaTypeId<std::pair<bool, int>>()
							  << QVariant::fromValue(std::pair<bool, int>{true, 42})
							  << QJsonValue{QJsonArray{42, true}};

	QTest::newRow("tuple.basic") << tupleConverter
								 << QVariantHash{}
								 << TestQ{{QMetaType::Int, 5, 1}, {QMetaType::Bool, true, 2}, {QMetaType::Double, 5.5, 3}}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<std::tuple<int, bool, double>>()
								 << QVariant::fromValue(std::make_tuple(5, true, 5.5))
								 << QJsonValue{QJsonArray{1, 2, 3}};

	QTest::newRow("object.basic") << objectConverter
								  << QVariantHash{}
								  << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
								  << static_cast<QObject*>(nullptr)
								  << qMetaTypeId<TestObject*>()
								  << QVariant::fromValue(new TestObject{10, 0.1, 11, this})
								  << QJsonValue{QJsonObject{
											{QStringLiteral("key"), 1},
											{QStringLiteral("value"), 2}
										}};
	QTest::newRow("object.tracking") << objectConverter
									 << QVariantHash{}
									 << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
									 << static_cast<QObject*>(nullptr)
									 << qMetaTypeId<QPointer<TestObject>>()
									 << QVariant::fromValue<QPointer<TestObject>>(new TestObject{10, 0.1, 11, this})
									 << QJsonValue{QJsonObject{
											   {QStringLiteral("key"), 1},
											   {QStringLiteral("value"), 2}
										   }};
	QTest::newRow("object.shared") << objectConverter
								   << QVariantHash{}
								   << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<QSharedPointer<TestObject>>()
								   << QVariant::fromValue(QSharedPointer<TestObject>::create(10, 0.1, 11))
								   << QJsonValue{QJsonObject{
											 {QStringLiteral("key"), 1},
											 {QStringLiteral("value"), 2}
										 }};
	QTest::newRow("object.null.basic") << objectConverter
									   << QVariantHash{}
									   << TestQ{}
									   << static_cast<QObject*>(nullptr)
									   << qMetaTypeId<TestObject*>()
									   << QVariant::fromValue<TestObject*>(nullptr)
									   << QJsonValue{QJsonValue::Null};
	QTest::newRow("object.null.tracking") << objectConverter
										  << QVariantHash{}
										  << TestQ{}
										  << static_cast<QObject*>(nullptr)
										  << qMetaTypeId<QPointer<TestObject>>()
										  << QVariant::fromValue<QPointer<TestObject>>(nullptr)
										  << QJsonValue{QJsonValue::Null};
	QTest::newRow("object.null.shared") << objectConverter
										<< QVariantHash{}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< qMetaTypeId<QSharedPointer<TestObject>>()
										<< QVariant::fromValue<QSharedPointer<TestObject>>(nullptr)
										<< QJsonValue{QJsonValue::Null};

//	QTest::newRow("object.poly.disabled") << objectConverter
//										  << QVariantHash{{QStringLiteral("polymorphing"), QVariant::fromValue<QJsonSerializer::Polymorphing>(QJsonSerializer::Disabled)}}
//										  << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
//										  << static_cast<QObject*>(nullptr)
//										  << qMetaTypeId<TestObject*>()
//										  << QVariant::fromValue(new TestObject{10, 0.1, 11, this})
//										  << QJsonValue{QJsonObject{
//													{QStringLiteral("key"), 1},
//													{QStringLiteral("value"), 2}
//												}};
	QTest::newRow("object.poly.enabled.static") << objectConverter
												<< QVariantHash{{QStringLiteral("polymorphing"), QVariant::fromValue<QJsonSerializer::Polymorphing>(QJsonSerializer::Enabled)}}
												<< TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}, {QMetaType::Bool, true, 3}}
												<< static_cast<QObject*>(nullptr)
												<< qMetaTypeId<TestObject*>()
												<< QVariant::fromValue<TestObject*>(new StaticPolyObject{10, 0.1, 11, true, this})
												<< QJsonValue{QJsonObject{
														  {QStringLiteral("@class"), QStringLiteral("StaticPolyObject")},
														  {QStringLiteral("key"), 1},
														  {QStringLiteral("value"), 2},
														  {QStringLiteral("extra"), 3}
													  }};
}

QTEST_MAIN(TypeConverterTest)

#include "tst_typeconverter.moc"
