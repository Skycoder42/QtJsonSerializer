#include <QtTest>
#include <QtJsonSerializer>

#include "dummyserializationhelper.h"

#include <QtJsonSerializer/private/qjsonbytearrayconverter_p.h>

Q_DECLARE_METATYPE(QSharedPointer<QJsonTypeConverter>)
Q_DECLARE_METATYPE(QJsonValue::Type)

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
	using TestQ = QQueue<DummySerializationHelper::SerInfo>;

	DummySerializationHelper *helper;

	QSharedPointer<QJsonTypeConverter> byteConverter;
};

void TypeConverterTest::initTestCase()
{
	helper = new DummySerializationHelper{this};

	byteConverter.reset(new QJsonBytearrayConverter{});
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

	QTest::newRow("bytearray") << byteConverter
							   << static_cast<int>(QJsonTypeConverter::Standard)
							   << QList<QJsonValue::Type>{QJsonValue::String};
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

	QTest::newRow("bytearray.bytearray") << byteConverter
										 << static_cast<int>(QMetaType::QByteArray)
										 << true;
	QTest::newRow("bytearray.string") << byteConverter
									  << static_cast<int>(QMetaType::QString)
									  << false;
	QTest::newRow("bytearray.char") << byteConverter
									<< static_cast<int>(QMetaType::Char)
									<< false;
	QTest::newRow("bytearray.string") << byteConverter
									  << static_cast<int>(QMetaType::Int)
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
	QTest::addColumn<QQueue<DummySerializationHelper::SerInfo>>("serData");
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QJsonValue>("result");

	QTest::newRow("bytearray.basic") << byteConverter
									 << QVariantHash{}
									 << TestQ{}
									 << static_cast<int>(QMetaType::QByteArray)
									 << QVariant{QByteArrayLiteral("Hello World")}
									 << QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ=")};
}

void TypeConverterTest::testSerialization()
{
	QFETCH(QSharedPointer<QJsonTypeConverter>, converter);
	QFETCH(QVariantHash, properties);
	QFETCH(QQueue<DummySerializationHelper::SerInfo>, serData);
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
	QTest::addColumn<QQueue<DummySerializationHelper::SerInfo>>("deserData");
	QTest::addColumn<int>("type");
	QTest::addColumn<QJsonValue>("data");
	QTest::addColumn<QVariant>("result");

	QTest::newRow("bytearray.basic") << byteConverter
									 << QVariantHash{}
									 << TestQ{}
									 << static_cast<int>(QMetaType::QByteArray)
									 << QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ=")}
									 << QVariant{QByteArrayLiteral("Hello World")};
	QTest::newRow("bytearray.invalid") << byteConverter
									   << QVariantHash{{QStringLiteral("validateBase64"), false}}
									   << TestQ{}
									   << static_cast<int>(QMetaType::QByteArray)
									   << QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ=42")}
									   << QVariant{QByteArrayLiteral("Hello World8")};
	QTest::newRow("bytearray.validated1") << byteConverter
										  << QVariantHash{{QStringLiteral("validateBase64"), true}}
										  << TestQ{}
										  << static_cast<int>(QMetaType::QByteArray)
										  << QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ=42")}
										  << QVariant{};
	QTest::newRow("bytearray.validated2") << byteConverter
										  << QVariantHash{{QStringLiteral("validateBase64"), true}}
										  << TestQ{}
										  << static_cast<int>(QMetaType::QByteArray)
										  << QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ2=")}
										  << QVariant{};
	QTest::newRow("bytearray.validated3") << byteConverter
										  << QVariantHash{{QStringLiteral("validateBase64"), true}}
										  << TestQ{}
										  << static_cast<int>(QMetaType::QByteArray)
										  << QJsonValue{QStringLiteral("SGVsbG%gV29ybGQ=")}
										  << QVariant{};
}

void TypeConverterTest::testDeserialization()
{
	QFETCH(QSharedPointer<QJsonTypeConverter>, converter);
	QFETCH(QVariantHash, properties);
	QFETCH(QQueue<DummySerializationHelper::SerInfo>, deserData);
	QFETCH(int, type);
	QFETCH(QJsonValue, data);
	QFETCH(QVariant, result);

	helper->properties = properties;
	helper->deserData = deserData;

	try {
		if(!result.isValid())
			QVERIFY_EXCEPTION_THROWN(converter->deserialize(type, data, this, helper), QJsonDeserializationException);
		else {
			auto res = converter->deserialize(type, data, this, helper);
			QCOMPARE(res, result);
		}
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

QTEST_MAIN(TypeConverterTest)

#include "tst_typeconverter.moc"
