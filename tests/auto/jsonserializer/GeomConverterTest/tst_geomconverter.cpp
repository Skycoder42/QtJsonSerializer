#include <QtTest>
#include <QtJsonSerializer>

#include "multitypeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsongeomconverter_p.h>

class GeomConverterTest : public MultiTypeConverterTestBase
{
	Q_OBJECT

protected:
	void addConverterInstances() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	QJsonSizeConverter _sizeConverter;
	QJsonPointConverter _pointConverter;
	QJsonLineConverter _lineConverter;
	QJsonRectConverter _rectConverter;

	QJsonTypeConverter *sizeConverter = &_sizeConverter;
	QJsonTypeConverter *pointConverter = &_pointConverter;
	QJsonTypeConverter *lineConverter = &_lineConverter;
	QJsonTypeConverter *rectConverter = &_rectConverter;
};

void GeomConverterTest::addConverterInstances()
{
	QTest::newRow("size") << sizeConverter;
	QTest::newRow("point") << pointConverter;
	QTest::newRow("line") << lineConverter;
	QTest::newRow("rect") << rectConverter;
}

void GeomConverterTest::addConverterData()
{
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

void GeomConverterTest::addMetaData()
{
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
}

void GeomConverterTest::addCommonSerData()
{
	QTest::newRow("size.int") << sizeConverter
							  << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QSize)
							  << QVariant{QSize{10, 20}}
							  << QJsonValue{QJsonObject{
										{QStringLiteral("width"), 10},
										{QStringLiteral("height"), 20}
									}};
	QTest::newRow("size.float") << sizeConverter
								<< QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QSizeF)
								<< QVariant{QSizeF{10.1, 20.2}}
								<< QJsonValue{QJsonObject{
										{QStringLiteral("width"), 10.1},
										{QStringLiteral("height"), 20.2}
									}};

	QTest::newRow("point.int") << pointConverter
							   << QVariantHash{}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QPoint)
							   << QVariant{QPoint{10, 20}}
							   << QJsonValue{QJsonObject{
										{QStringLiteral("x"), 10},
										{QStringLiteral("y"), 20}
									}};
	QTest::newRow("point.float") << pointConverter
								 << QVariantHash{}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
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
							  << static_cast<QObject*>(this)
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
								 << static_cast<QObject*>(this)
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
							  << static_cast<QObject*>(this)
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
								 << static_cast<QObject*>(this)
								 << static_cast<int>(QMetaType::QRectF)
								 << QVariant{QRectF{QPointF{10.1, 10.1}, QPointF{20.2, 20.2}}}
								 << QJsonValue{QJsonObject{
										{QStringLiteral("topLeft"), 42},
										{QStringLiteral("bottomRight"), 42}
									}};
}

void GeomConverterTest::addDeserData()
{
	QTest::newRow("size.invalid") << sizeConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QSize)
								  << QVariant{}
								  << QJsonValue{QJsonObject{
											{QStringLiteral("width"), 42},
											{QStringLiteral("haight"), 42}
										}};

	QTest::newRow("point.invalid") << pointConverter
								   << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QPoint)
								   << QVariant{}
								   << QJsonValue{QJsonObject{
											{QStringLiteral("x"), 42},
											{QStringLiteral("z"), 42}
										}};

	QTest::newRow("line.invalid") << lineConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QLine)
								  << QVariant{}
								  << QJsonValue{QJsonObject{
											{QStringLiteral("p1"), 42},
											{QStringLiteral("p3"), 42}
										}};

	QTest::newRow("rect.invalid") << rectConverter
								  << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QRect)
								  << QVariant{}
								  << QJsonValue{QJsonObject{
											{QStringLiteral("left"), 42},
											{QStringLiteral("right"), 42}
										}};
}

QTEST_MAIN(GeomConverterTest)

#include "tst_geomconverter.moc"
