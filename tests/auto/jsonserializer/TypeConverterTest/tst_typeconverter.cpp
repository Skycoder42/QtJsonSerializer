#include <QtTest>
#include <QtJsonSerializer>

#include "dummyserializationhelper.h"

#include <QtJsonSerializer/private/qjsonbytearrayconverter_p.h>
#include <QtJsonSerializer/private/qjsongeomconverter_p.h>

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
	using TestQ = QList<DummySerializationHelper::SerInfo>;

	DummySerializationHelper *helper;

	QSharedPointer<QJsonTypeConverter> byteConverter;
	QSharedPointer<QJsonTypeConverter> sizeConverter;
	QSharedPointer<QJsonTypeConverter> pointConverter;
	QSharedPointer<QJsonTypeConverter> lineConverter;
	QSharedPointer<QJsonTypeConverter> rectConverter;

	void addCommonSerData();
};

void TypeConverterTest::initTestCase()
{
	helper = new DummySerializationHelper{this};

	byteConverter.reset(new QJsonBytearrayConverter{});
	sizeConverter.reset(new QJsonSizeConverter{});
	pointConverter.reset(new QJsonPointConverter{});
	lineConverter.reset(new QJsonLineConverter{});
	rectConverter.reset(new QJsonRectConverter{});
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

	QTest::newRow("size") << sizeConverter
						  << static_cast<int>(QJsonTypeConverter::Standard)
						  << QList<QJsonValue::Type>{QJsonValue::Object};

	QTest::newRow("point") << pointConverter
						   << static_cast<int>(QJsonTypeConverter::Standard)
						   << QList<QJsonValue::Type>{QJsonValue::Object};

	QTest::newRow("line") << lineConverter
						  << static_cast<int>(QJsonTypeConverter::Standard)
						  << QList<QJsonValue::Type>{QJsonValue::Object};

	QTest::newRow("rect") << rectConverter
						  << static_cast<int>(QJsonTypeConverter::Standard)
						  << QList<QJsonValue::Type>{QJsonValue::Object};
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

	QTest::newRow("size.size") << sizeConverter
							   << static_cast<int>(QMetaType::QSize)
							   << true;
	QTest::newRow("size.sizef") << sizeConverter
								<< static_cast<int>(QMetaType::QSizeF)
								<< true;
	QTest::newRow("size.point") << sizeConverter
								<< static_cast<int>(QMetaType::QPoint)
								<< false;

	QTest::newRow("point.point") << pointConverter
								 << static_cast<int>(QMetaType::QPoint)
								 << true;
	QTest::newRow("point.pointf") << pointConverter
								  << static_cast<int>(QMetaType::QPointF)
								  << true;
	QTest::newRow("point.size") << pointConverter
								<< static_cast<int>(QMetaType::QSize)
								<< false;

	QTest::newRow("line.line") << lineConverter
							   << static_cast<int>(QMetaType::QLine)
							   << true;
	QTest::newRow("line.linef") << lineConverter
								<< static_cast<int>(QMetaType::QLineF)
								<< true;
	QTest::newRow("line.point") << lineConverter
								<< static_cast<int>(QMetaType::QPoint)
								<< false;

	QTest::newRow("rect.rect") << rectConverter
							   << static_cast<int>(QMetaType::QRect)
							   << true;
	QTest::newRow("rect.rectf") << rectConverter
								<< static_cast<int>(QMetaType::QRectF)
								<< true;
	QTest::newRow("rect.line") << rectConverter
							   << static_cast<int>(QMetaType::QLine)
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
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QJsonValue>("result");

	addCommonSerData();
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
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("result");
	QTest::addColumn<QJsonValue>("data");

	addCommonSerData();

	QTest::newRow("bytearray.invalid") << byteConverter
									   << QVariantHash{{QStringLiteral("validateBase64"), false}}
									   << TestQ{}
									   << static_cast<int>(QMetaType::QByteArray)
									   << QVariant{QByteArrayLiteral("Hello World8")}
									   << QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ=42")};
	QTest::newRow("bytearray.validated1") << byteConverter
										  << QVariantHash{{QStringLiteral("validateBase64"), true}}
										  << TestQ{}
										  << static_cast<int>(QMetaType::QByteArray)
										  << QVariant{}
										  << QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ=42")};
	QTest::newRow("bytearray.validated2") << byteConverter
										  << QVariantHash{{QStringLiteral("validateBase64"), true}}
										  << TestQ{}
										  << static_cast<int>(QMetaType::QByteArray)
										  << QVariant{}
										  << QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ2=")};
	QTest::newRow("bytearray.validated3") << byteConverter
										  << QVariantHash{{QStringLiteral("validateBase64"), true}}
										  << TestQ{}
										  << static_cast<int>(QMetaType::QByteArray)
										  << QVariant{}
										  << QJsonValue{QStringLiteral("SGVsbG%gV29ybGQ=")};
}

void TypeConverterTest::testDeserialization()
{
	QFETCH(QSharedPointer<QJsonTypeConverter>, converter);
	QFETCH(QVariantHash, properties);
	QFETCH(QList<DummySerializationHelper::SerInfo>, deserData);
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

void TypeConverterTest::addCommonSerData()
{

	QTest::newRow("bytearray.basic") << byteConverter
									 << QVariantHash{}
									 << TestQ{}
									 << static_cast<int>(QMetaType::QByteArray)
									 << QVariant{QByteArrayLiteral("Hello World")}
									 << QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ=")};

	QTest::newRow("size.int") << sizeConverter
							  << QVariantHash{}
							  << TestQ{}
							  << static_cast<int>(QMetaType::QSize)
							  << QVariant{QSize{10, 20}}
							  << QJsonValue{QJsonObject{
										{QStringLiteral("width"), 10},
										{QStringLiteral("height"), 20}
									}};
	QTest::newRow("size.float") << sizeConverter
								<< QVariantHash{}
								<< TestQ{}
								<< static_cast<int>(QMetaType::QSizeF)
								<< QVariant{QSizeF{10.1, 20.2}}
								<< QJsonValue{QJsonObject{
										{QStringLiteral("width"), 10.1},
										{QStringLiteral("height"), 20.2}
									}};

	QTest::newRow("point.int") << pointConverter
							   << QVariantHash{}
							   << TestQ{}
							   << static_cast<int>(QMetaType::QPoint)
							   << QVariant{QPoint{10, 20}}
							   << QJsonValue{QJsonObject{
										{QStringLiteral("x"), 10},
										{QStringLiteral("y"), 20}
									}};
	QTest::newRow("point.float") << pointConverter
								 << QVariantHash{}
								 << TestQ{}
								 << static_cast<int>(QMetaType::QPointF)
								 << QVariant{QPointF{10.1, 20.2}}
								 << QJsonValue{QJsonObject{
										{QStringLiteral("x"), 10.1},
										{QStringLiteral("y"), 20.2}
									}};

	QTest::newRow("line.int") << lineConverter
							  << QVariantHash{}
							  << TestQ{
										{QMetaType::QPoint, QVariant{QPoint{10, 10}}, QJsonValue(42)},
										{QMetaType::QPoint, QVariant{QPoint{20, 20}}, QJsonValue(42)}
									}
							  << static_cast<int>(QMetaType::QLine)
							  << QVariant{QLine{QPoint{10, 10}, QPoint{20, 20}}}
							  << QJsonValue{QJsonObject{
										{QStringLiteral("p1"), 42},
										{QStringLiteral("p2"), 42}
									}};
	QTest::newRow("line.floatf") << lineConverter
								 << QVariantHash{}
								 << TestQ{
										{QMetaType::QPointF, QVariant{QPointF{10.1, 10.1}}, QJsonValue(42)},
										{QMetaType::QPointF, QVariant{QPointF{20.2, 20.2}}, QJsonValue(42)}
									}
								 << static_cast<int>(QMetaType::QLineF)
								 << QVariant{QLineF{QPointF{10.1, 10.1}, QPointF{20.2, 20.2}}}
								 << QJsonValue{QJsonObject{
										{QStringLiteral("p1"), 42},
										{QStringLiteral("p2"), 42}
									}};

	QTest::newRow("rect.int") << rectConverter
							  << QVariantHash{}
							  << TestQ{
										{QMetaType::QPoint, QVariant{QPoint{10, 10}}, QJsonValue(42)},
										{QMetaType::QPoint, QVariant{QPoint{20, 20}}, QJsonValue(42)}
									}
							  << static_cast<int>(QMetaType::QRect)
							  << QVariant{QRect{QPoint{10, 10}, QPoint{20, 20}}}
							  << QJsonValue{QJsonObject{
										{QStringLiteral("topLeft"), 42},
										{QStringLiteral("bottomRight"), 42}
									}};
	QTest::newRow("rect.floatf") << rectConverter
								 << QVariantHash{}
								 << TestQ{
										{QMetaType::QPointF, QVariant{QPointF{10.1, 10.1}}, QJsonValue(42)},
										{QMetaType::QPointF, QVariant{QPointF{20.2, 20.2}}, QJsonValue(42)}
									}
								 << static_cast<int>(QMetaType::QRectF)
								 << QVariant{QRectF{QPointF{10.1, 10.1}, QPointF{20.2, 20.2}}}
								 << QJsonValue{QJsonObject{
										{QStringLiteral("topLeft"), 42},
										{QStringLiteral("bottomRight"), 42}
									}};
}

QTEST_MAIN(TypeConverterTest)

#include "tst_typeconverter.moc"
