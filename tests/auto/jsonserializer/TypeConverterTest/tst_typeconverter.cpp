#include <QtTest>
#include <QtJsonSerializer>

#include "dummyserializationhelper.h"

#include <QtJsonSerializer/private/qjsonbytearrayconverter_p.h>
#include <QtJsonSerializer/private/qjsongeomconverter_p.h>
#include <QtJsonSerializer/private/qjsonjsonconverter_p.h>
#include <QtJsonSerializer/private/qjsonlocaleconverter_p.h>

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
	QSharedPointer<QJsonTypeConverter> jValConverter;
	QSharedPointer<QJsonTypeConverter> jObjConverter;
	QSharedPointer<QJsonTypeConverter> jArrConverter;
	QSharedPointer<QJsonTypeConverter> localeConverter;

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
	jValConverter.reset(new QJsonJsonValueConverter{});
	jObjConverter.reset(new QJsonJsonObjectConverter{});
	jArrConverter.reset(new QJsonJsonArrayConverter{});
	localeConverter.reset(new QJsonLocaleConverter{});
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

	QTest::newRow("json.value") << jValConverter
								<< static_cast<int>(QJsonTypeConverter::Standard)
								<< QList<QJsonValue::Type>{QJsonValue::Null, QJsonValue::Bool, QJsonValue::Double, QJsonValue::String, QJsonValue::Array, QJsonValue::Object};

	QTest::newRow("json.object") << jObjConverter
								 << static_cast<int>(QJsonTypeConverter::Standard)
								 << QList<QJsonValue::Type>{QJsonValue::Object};

	QTest::newRow("json.array") << jArrConverter
								<< static_cast<int>(QJsonTypeConverter::Standard)
								<< QList<QJsonValue::Type>{QJsonValue::Array};

	QTest::newRow("locale") << localeConverter
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
	QTest::newRow("bytearray.invalid.string") << byteConverter
											  << static_cast<int>(QMetaType::QString)
											  << false;
	QTest::newRow("bytearray.invalid.char") << byteConverter
											<< static_cast<int>(QMetaType::Char)
											<< false;
	QTest::newRow("bytearray.invalid.string") << byteConverter
											  << static_cast<int>(QMetaType::Int)
											  << false;

	QTest::newRow("size.size") << sizeConverter
							   << static_cast<int>(QMetaType::QSize)
							   << true;
	QTest::newRow("size.sizef") << sizeConverter
								<< static_cast<int>(QMetaType::QSizeF)
								<< true;
	QTest::newRow("size.invalid") << sizeConverter
								  << static_cast<int>(QMetaType::QPoint)
								  << false;

	QTest::newRow("point.point") << pointConverter
								 << static_cast<int>(QMetaType::QPoint)
								 << true;
	QTest::newRow("point.pointf") << pointConverter
								  << static_cast<int>(QMetaType::QPointF)
								  << true;
	QTest::newRow("point.invalid") << pointConverter
								   << static_cast<int>(QMetaType::QSize)
								   << false;

	QTest::newRow("line.line") << lineConverter
							   << static_cast<int>(QMetaType::QLine)
							   << true;
	QTest::newRow("line.linef") << lineConverter
								<< static_cast<int>(QMetaType::QLineF)
								<< true;
	QTest::newRow("line.invalid") << lineConverter
								  << static_cast<int>(QMetaType::QPoint)
								  << false;

	QTest::newRow("rect.rect") << rectConverter
							   << static_cast<int>(QMetaType::QRect)
							   << true;
	QTest::newRow("rect.rectf") << rectConverter
								<< static_cast<int>(QMetaType::QRectF)
								<< true;
	QTest::newRow("rect.invalid") << rectConverter
								  << static_cast<int>(QMetaType::QLine)
								  << false;

	QTest::newRow("json.value.value") << jValConverter
									  << static_cast<int>(QMetaType::QJsonValue)
									  << true;
	QTest::newRow("json.value.invalid") << jValConverter
										<< static_cast<int>(QMetaType::QJsonObject)
										<< false;

	QTest::newRow("json.object.object") << jObjConverter
										<< static_cast<int>(QMetaType::QJsonObject)
										<< true;
	QTest::newRow("json.object.invalid") << jObjConverter
										 << static_cast<int>(QMetaType::QJsonArray)
										 << false;

	QTest::newRow("json.array.array") << jArrConverter
									  << static_cast<int>(QMetaType::QJsonArray)
									  << true;
	QTest::newRow("json.array.invalid") << jArrConverter
										<< static_cast<int>(QMetaType::QJsonObject)
										<< false;

	QTest::newRow("locale.locale") << localeConverter
								   << static_cast<int>(QMetaType::QLocale)
								   << true;
	QTest::newRow("locale.invalid") << localeConverter
									<< static_cast<int>(QMetaType::QString)
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

	QTest::newRow("size.invalid") << sizeConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<int>(QMetaType::QSize)
								  << QVariant{}
								  << QJsonValue{QJsonObject{
											{QStringLiteral("width"), 42},
											{QStringLiteral("haight"), 42}
										}};

	QTest::newRow("point.invalid") << pointConverter
								   << QVariantHash{}
								   << TestQ{}
								   << static_cast<int>(QMetaType::QPoint)
								   << QVariant{}
								   << QJsonValue{QJsonObject{
											{QStringLiteral("x"), 42},
											{QStringLiteral("z"), 42}
										}};

	QTest::newRow("line.invalid") << lineConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<int>(QMetaType::QLine)
								  << QVariant{}
								  << QJsonValue{QJsonObject{
											{QStringLiteral("p1"), 42},
											{QStringLiteral("p3"), 42}
										}};

	QTest::newRow("rect.invalid") << rectConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<int>(QMetaType::QRect)
								  << QVariant{}
								  << QJsonValue{QJsonObject{
											{QStringLiteral("left"), 42},
											{QStringLiteral("right"), 42}
										}};

	QTest::newRow("locale.empty") << localeConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<int>(QMetaType::QLocale)
								  << QVariant{QLocale::c()}
								  << QJsonValue{QString{}};
	QTest::newRow("locale.invalid") << localeConverter
									<< QVariantHash{}
									<< TestQ{}
									<< static_cast<int>(QMetaType::QLocale)
									<< QVariant{}
									<< QJsonValue{QStringLiteral("some random text")};
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

	QTest::newRow("json.value") << jValConverter
								<< QVariantHash{}
								<< TestQ{}
								<< static_cast<int>(QMetaType::QJsonValue)
								<< QVariant{QJsonValue{42}}
								<< QJsonValue{42};

	QTest::newRow("json.object") << jObjConverter
								 << QVariantHash{}
								 << TestQ{}
								 << static_cast<int>(QMetaType::QJsonObject)
								 << QVariant{QJsonObject{{QStringLiteral("baum"), 42}}}
								 << QJsonValue{QJsonObject{{QStringLiteral("baum"), 42}}};

	QTest::newRow("json.array") << jArrConverter
								<< QVariantHash{}
								<< TestQ{}
								<< static_cast<int>(QMetaType::QJsonArray)
								<< QVariant{QJsonArray{10, 20, true, 4.2}}
								<< QJsonValue{QJsonArray{10, 20, true, 4.2}};

	QTest::newRow("locale.normal") << localeConverter
								   << QVariantHash{}
								   << TestQ{}
								   << static_cast<int>(QMetaType::QLocale)
								   << QVariant{QLocale{QLocale::German, QLocale::Germany}}
								   << QJsonValue{QStringLiteral("de_DE")};
	QTest::newRow("locale.c") << localeConverter
							  << QVariantHash{}
							  << TestQ{}
							  << static_cast<int>(QMetaType::QLocale)
							  << QVariant{QLocale::c()}
							  << QJsonValue{QStringLiteral("C")};
	QTest::newRow("locale.bcp47.default") << localeConverter
										  << QVariantHash{{QStringLiteral("useBcp47Locale"), true}}
										  << TestQ{}
										  << static_cast<int>(QMetaType::QLocale)
										  << QVariant{QLocale{QLocale::German, QLocale::Germany}}
										  << QJsonValue{QStringLiteral("de")};
	QTest::newRow("locale.bcp47.special") << localeConverter
										  << QVariantHash{{QStringLiteral("useBcp47Locale"), true}}
										  << TestQ{}
										  << static_cast<int>(QMetaType::QLocale)
										  << QVariant{QLocale{QLocale::German, QLocale::Austria}}
										  << QJsonValue{QStringLiteral("de-AT")};
}

QTEST_MAIN(TypeConverterTest)

#include "tst_typeconverter.moc"
