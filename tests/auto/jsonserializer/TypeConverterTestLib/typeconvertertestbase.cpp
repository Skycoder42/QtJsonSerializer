#include "typeconvertertestbase.h"
#include <QtJsonSerializer>
#include <QtTest>
#include <typeinfo>

#define private public
#include <QtJsonSerializer/private/serializerbase_p.h>
#undef private

using namespace QtJsonSerializer;

#define SELF_COMPARE(type, actual, expected) \
do {\
	if (!compare(type, actual, expected, #actual, #expected, __FILE__, __LINE__))\
		return;\
} while (false)

TypeConverterTestBase::TypeConverterTestBase(QObject *parent) :
	QObject{parent},
	helper{new DummySerializationHelper{this}}
{}

void TypeConverterTestBase::initTest() {}

void TypeConverterTestBase::cleanupTest() {}

void TypeConverterTestBase::addCommonSerData() {}

void TypeConverterTestBase::addSerData() {}

void TypeConverterTestBase::addDeserData() {}

bool TypeConverterTestBase::compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line)
{
	Q_UNUSED(type)
	return QTest::qCompare(actual, expected, aName, eName, file, line);
}

void TypeConverterTestBase::initTestCase()
{
	initTest();
}

void TypeConverterTestBase::cleanupTestCase()
{
	cleanupTest();
}

void TypeConverterTestBase::testConverterIsRegistered_data() {}

void TypeConverterTestBase::testConverterIsRegistered()
{
	const auto cOwn = converter();
	for(const auto &factory : qAsConst(SerializerBasePrivate::typeConverterFactories)) {
		const auto conv = factory->createConverter();
		const auto cReg = conv.data();
		if(typeid(*cOwn).hash_code() == typeid(*cReg).hash_code())
			return;
	}
	QFAIL(QByteArrayLiteral("TypeConverter \"") + cOwn->name() + QByteArrayLiteral("\" is not registerd within QJsonSerializer"));
}

void TypeConverterTestBase::testConverterMeta_data()
{
	QTest::addColumn<int>("priority");

	addConverterData();
}

void TypeConverterTestBase::testConverterMeta()
{
	QFETCH(int, priority);

	QCOMPARE(converter()->priority(), priority);
}

void TypeConverterTestBase::testMetaTypeDetection_data()
{
	QTest::addColumn<int>("metatype");
	QTest::addColumn<QCborTag>("tag");
	QTest::addColumn<QCborValue::Type>("type");
	QTest::addColumn<bool>("canSer");
	QTest::addColumn<TypeConverter::DeserializationCapabilityResult>("canDeser");

	addMetaData();
}

void TypeConverterTestBase::testMetaTypeDetection()
{
	QFETCH(int, metatype);
	QFETCH(QCborTag, tag);
	QFETCH(QCborValue::Type, type);
	QFETCH(bool, canSer);
	QFETCH(TypeConverter::DeserializationCapabilityResult, canDeser);

	helper->json = tag == static_cast<QCborTag>(CborSerializer::NoTag);
	converter()->setHelper(helper);
	QCOMPARE(converter()->canConvert(metatype), canSer);
	QCOMPARE(converter()->canDeserialize(metatype, tag, type), canDeser);
}

void TypeConverterTestBase::testSerialization_data()
{
	QTest::addColumn<QVariantHash>("properties");
	QTest::addColumn<QList<DummySerializationHelper::SerInfo>>("serData");
	QTest::addColumn<QObject*>("parent");
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QCborValue>("cResult");
	QTest::addColumn<QJsonValue>("jResult");

	addCommonSerData();
	addSerData();
}

void TypeConverterTestBase::testSerialization()
{
	QFETCH(QVariantHash, properties);
	QFETCH(QList<DummySerializationHelper::SerInfo>, serData);
	QFETCH(int, type);
	QFETCH(QVariant, data);
	QFETCH(QCborValue, cResult);
	QFETCH(QJsonValue, jResult);

	helper->properties = properties;
	converter()->setHelper(helper);

	try {
		if(cResult.isUndefined() && jResult.isUndefined()) {
			helper->json = false;
			helper->serData = serData;
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
			QVERIFY_EXCEPTION_THROWN(converter()->serialize(type, data), SerializationException);
#else
			QVERIFY_THROWS_EXCEPTION(SerializationException, converter()->serialize(type, data));
#endif
			helper->json = true;
			helper->serData = serData;
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
			QVERIFY_EXCEPTION_THROWN(converter()->serialize(type, data), SerializationException);
#else
			QVERIFY_THROWS_EXCEPTION(SerializationException, converter()->serialize(type, data));
#endif
		} else {
			if (!cResult.isUndefined()) {
				helper->json = false;
				helper->serData = serData;
				auto res = converter()->serialize(type, data);
				QCOMPARE(res, cResult);
			}
			if (!jResult.isUndefined()) {
				helper->json = true;
				helper->serData = serData;
				auto cRes = converter()->serialize(type, data);
				if (const auto tag = helper->typeTag(type); tag != static_cast<QCborTag>(CborSerializer::NoTag))
					cRes = {tag, cRes.isTag() ? cRes.taggedValue() : cRes};
				const auto res = cRes.toJsonValue();
				QCOMPARE(res, jResult);
			}
		}
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void TypeConverterTestBase::testDeserialization_data()
{
	QTest::addColumn<QVariantHash>("properties");
	QTest::addColumn<QList<DummySerializationHelper::SerInfo>>("deserData");
	QTest::addColumn<QObject*>("parent");
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("result");
	QTest::addColumn<QCborValue>("cData");
	QTest::addColumn<QJsonValue>("jData");

	addCommonSerData();
	addDeserData();
}

void TypeConverterTestBase::testDeserialization()
{
	QFETCH(QVariantHash, properties);
	QFETCH(QList<DummySerializationHelper::SerInfo>, deserData);
	QFETCH(QObject*, parent);
	QFETCH(int, type);
	QFETCH(QCborValue, cData);
	QFETCH(QJsonValue, jData);
	QFETCH(QVariant, result);

	helper->properties = properties;
	helper->expectedParent = parent;
	converter()->setHelper(helper);

	try {
		if(!result.isValid()) {
			if (!cData.isUndefined()) {
				helper->json = false;
				helper->deserData = deserData;
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
				QVERIFY_EXCEPTION_THROWN(converter()->deserializeCbor(type, cData, this), DeserializationException);
#else
				QVERIFY_THROWS_EXCEPTION(DeserializationException, converter()->deserializeCbor(type, cData, this));
#endif
			}
			if (!jData.isUndefined()) {
				helper->json = true;
				helper->deserData = deserData;
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
				QVERIFY_EXCEPTION_THROWN(converter()->deserializeJson(type, QCborValue::fromJsonValue(jData), this), DeserializationException);
#else
				QVERIFY_THROWS_EXCEPTION(DeserializationException, converter()->deserializeJson(type, QCborValue::fromJsonValue(jData), this));
#endif
			}
		} else {
			if (!cData.isUndefined()) {
				helper->json = false;
				helper->deserData = deserData;
				auto res = converter()->deserializeCbor(type, cData, this);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
				QVERIFY(res.convert(type));
#else
				QVERIFY(res.convert(QMetaType(type)));
#endif
				SELF_COMPARE(type, res, result);
			}
			if (!jData.isUndefined()) {
				helper->json = true;
				helper->deserData = deserData;
				auto res = converter()->deserializeJson(type, QCborValue::fromJsonValue(jData), this);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
				QVERIFY(res.convert(type));
#else
				QVERIFY(res.convert(QMetaType(type)));
#endif
				SELF_COMPARE(type, res, result);
			}
		}
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}
