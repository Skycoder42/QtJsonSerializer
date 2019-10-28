#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonlegacygeomconverter_p.h>

class LegacyGeomConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addSerData() override;
	void addDeserData() override;

private:
	QJsonLegacyGeomConverter _converter;
};

QJsonTypeConverter *LegacyGeomConverterTest::converter()
{
	return &_converter;
}

void LegacyGeomConverterTest::addConverterData()
{
	QTest::newRow("geom") << static_cast<int>(QJsonTypeConverter::VeryLow);
}

void LegacyGeomConverterTest::addMetaData()
{
	QTest::newRow("size") << static_cast<int>(QMetaType::QSize)
						  << static_cast<QCborTag>(QCborSerializer::NoTag)
						  << QCborValue::Map
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("sizef") << static_cast<int>(QMetaType::QSizeF)
						   << static_cast<QCborTag>(QCborSerializer::NoTag)
						   << QCborValue::Map
						   << true
						   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("size.invalid") << static_cast<int>(QMetaType::QSize)
								  << static_cast<QCborTag>(QCborSerializer::NoTag)
								  << QCborValue::Array
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("point") << static_cast<int>(QMetaType::QPoint)
						   << static_cast<QCborTag>(QCborSerializer::NoTag)
						   << QCborValue::Map
						   << true
						   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("pointf") << static_cast<int>(QMetaType::QPointF)
							<< static_cast<QCborTag>(QCborSerializer::NoTag)
							<< QCborValue::Map
							<< true
							<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("point.invalid") << static_cast<int>(QMetaType::QPoint)
								   << static_cast<QCborTag>(QCborSerializer::NoTag)
								   << QCborValue::Array
								   << true
								   << QJsonTypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("line") << static_cast<int>(QMetaType::QLine)
						  << static_cast<QCborTag>(QCborSerializer::NoTag)
						  << QCborValue::Map
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("linef") << static_cast<int>(QMetaType::QLine)
						   << static_cast<QCborTag>(QCborSerializer::NoTag)
						   << QCborValue::Map
						   << true
						   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("line.invalid") << static_cast<int>(QMetaType::QLine)
								  << static_cast<QCborTag>(QCborSerializer::NoTag)
								  << QCborValue::Array
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("rect") << static_cast<int>(QMetaType::QRect)
						  << static_cast<QCborTag>(QCborSerializer::NoTag)
						  << QCborValue::Map
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("rectf") << static_cast<int>(QMetaType::QRectF)
						   << static_cast<QCborTag>(QCborSerializer::NoTag)
						   << QCborValue::Map
						   << true
						   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("rect.invalid") << static_cast<int>(QMetaType::QRect)
								  << static_cast<QCborTag>(QCborSerializer::NoTag)
								  << QCborValue::Array
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QMatrix)
								  << static_cast<QCborTag>(QCborSerializer::NoTag)
								  << QCborValue::Array
								  << false
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
}

void LegacyGeomConverterTest::addSerData()
{
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << static_cast<int>(QMetaType::QSize)
							 << QVariant{QSize{10, 20}}
							 << QCborValue{}
							 << QJsonValue{QJsonValue::Undefined};
}

void LegacyGeomConverterTest::addDeserData()
{
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << static_cast<int>(QMetaType::QSize)
							 << QVariant{}
							 << QCborValue{QCborMap{
									{QStringLiteral("width"), 10},
									{QStringLiteral("height"), 20}
								}}
							 << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("size.int") << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QSize)
							  << QVariant{QSize{10, 20}}
							  << QCborValue{}
							  << QJsonValue{QJsonObject{
									 {QStringLiteral("width"), 10},
									 {QStringLiteral("height"), 20}
								 }};
	QTest::newRow("size.real") << QVariantHash{}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QSizeF)
							   << QVariant{QSizeF{10.1, 20.2}}
							   << QCborValue{}
							   << QJsonValue{QJsonObject{
									  {QStringLiteral("width"), 10.1},
									  {QStringLiteral("height"), 20.2}
								  }};

	QTest::newRow("point.int") << QVariantHash{}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QPoint)
							   << QVariant{QPoint{10, 20}}
							   << QCborValue{}
							   << QJsonValue{QJsonObject{
									  {QStringLiteral("x"), 10},
									  {QStringLiteral("y"), 20}
								  }};
	QTest::newRow("point.real") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QPointF)
								<< QVariant{QPointF{10.1, 20.2}}
								<< QCborValue{}
								<< QJsonValue{QJsonObject{
									   {QStringLiteral("x"), 10.1},
									   {QStringLiteral("y"), 20.2}
								   }};

	QTest::newRow("line.int") << QVariantHash{}
							  << TestQ{
									 {QMetaType::QPoint, QVariant{QPoint{10, 10}}, 24},
									 {QMetaType::QPoint, QVariant{QPoint{20, 20}}, 42}
								 }
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QLine)
							  << QVariant{QLine{QPoint{10, 10}, QPoint{20, 20}}}
							  << QCborValue{}
							  << QJsonValue{QJsonObject{
									 {QStringLiteral("p1"), 24},
									 {QStringLiteral("p2"), 42}
								 }};
	QTest::newRow("line.real") << QVariantHash{}
							   << TestQ{
									  {QMetaType::QPointF, QVariant{QPointF{10.1, 10.1}}, 2.4},
									  {QMetaType::QPointF, QVariant{QPointF{20.2, 20.2}}, 4.2}
								  }
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QLineF)
							   << QVariant{QLineF{QPointF{10.1, 10.1}, QPointF{20.2, 20.2}}}
							   << QCborValue{}
							   << QJsonValue{QJsonObject{
									  {QStringLiteral("p1"), 2.4},
									  {QStringLiteral("p2"), 4.2}
								  }};

	QTest::newRow("rect.int") << QVariantHash{}
							  << TestQ{
									 {QMetaType::QPoint, QVariant{QPoint{10, 10}}, 24},
									 {QMetaType::QPoint, QVariant{QPoint{20, 20}}, 42}
								 }
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QRect)
							  << QVariant{QRect{QPoint{10, 10}, QPoint{20, 20}}}
							  << QCborValue{}
							  << QJsonValue{QJsonObject{
									 {QStringLiteral("topLeft"), 24},
									 {QStringLiteral("bottomRight"), 42}
								 }};
	QTest::newRow("rect.real") << QVariantHash{}
							   << TestQ{
									  {QMetaType::QPointF, QVariant{QPointF{10.1, 10.1}}, 2.4},
									  {QMetaType::QPointF, QVariant{QPointF{20.2, 20.2}}, 4.2}
								  }
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QRectF)
							   << QVariant{QRectF{QPointF{10.1, 10.1}, QPointF{20.2, 20.2}}}
							   << QCborValue{}
							   << QJsonValue{QJsonObject{
									  {QStringLiteral("topLeft"), 2.4},
									  {QStringLiteral("bottomRight"), 4.2}
								  }};

	QTest::newRow("size.invalid") << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QSize)
								  << QVariant{}
								  << QCborValue{}
								  << QJsonValue{QJsonObject{
										 {QStringLiteral("width"), 42},
										 {QStringLiteral("haight"), 42}
									 }};

	QTest::newRow("point.invalid") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QPoint)
								   << QVariant{}
								   << QCborValue{}
								   << QJsonValue{QJsonObject{
										  {QStringLiteral("x"), 42},
										  {QStringLiteral("z"), 42}
									  }};

	QTest::newRow("line.invalid") << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QLine)
								  << QVariant{}
								  << QCborValue{}
								  << QJsonValue{QJsonObject{
										 {QStringLiteral("p1"), 42},
										 {QStringLiteral("p3"), 42}
									 }};

	QTest::newRow("rect.invalid") << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QRect)
								  << QVariant{}
								  << QCborValue{}
								  << QJsonValue{QJsonObject{
										 {QStringLiteral("left"), 42},
										 {QStringLiteral("right"), 42}
									 }};
}

QTEST_MAIN(LegacyGeomConverterTest)

#include "tst_legacygeomconverter.moc"
