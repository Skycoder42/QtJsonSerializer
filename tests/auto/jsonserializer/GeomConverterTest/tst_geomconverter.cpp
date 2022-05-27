#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/geomconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class GeomConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	GeomConverter _converter;
};

TypeConverter *GeomConverterTest::converter()
{
	return &_converter;
}

void GeomConverterTest::addConverterData()
{
	QTest::newRow("geom") << static_cast<int>(TypeConverter::Standard);
}

void GeomConverterTest::addMetaData()
{
	QTest::newRow("size.tagged") << static_cast<int>(QMetaType::QSize)
								 << static_cast<QCborTag>(CborSerializer::GeomSize)
								 << QCborValue::Array
								 << true
								 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("size.untagged") << static_cast<int>(QMetaType::QSize)
								   << static_cast<QCborTag>(CborSerializer::NoTag)
								   << QCborValue::Array
								   << true
								   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("sizef.tagged") << static_cast<int>(QMetaType::QSizeF)
								  << static_cast<QCborTag>(CborSerializer::GeomSize)
								  << QCborValue::Array
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("sizef.untagged") << static_cast<int>(QMetaType::QSizeF)
									<< static_cast<QCborTag>(CborSerializer::NoTag)
									<< QCborValue::Array
									<< true
									<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("size.guessed") << static_cast<int>(QMetaType::UnknownType)
								  << static_cast<QCborTag>(CborSerializer::GeomSize)
								  << QCborValue::Array
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("size.invalid.data") << static_cast<int>(QMetaType::QSize)
									   << static_cast<QCborTag>(CborSerializer::GeomSize)
									   << QCborValue::Map
									   << true
									   << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("size.invalid.tag") << static_cast<int>(QMetaType::QSizeF)
									  << static_cast<QCborTag>(CborSerializer::GeomPoint)
									  << QCborValue::Map
									  << true
									  << TypeConverter::DeserializationCapabilityResult::WrongTag;

	QTest::newRow("point.tagged") << static_cast<int>(QMetaType::QPoint)
								  << static_cast<QCborTag>(CborSerializer::GeomPoint)
								  << QCborValue::Array
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("point.untagged") << static_cast<int>(QMetaType::QPoint)
									<< static_cast<QCborTag>(CborSerializer::NoTag)
									<< QCborValue::Array
									<< true
									<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("pointf.tagged") << static_cast<int>(QMetaType::QPointF)
								   << static_cast<QCborTag>(CborSerializer::GeomPoint)
								   << QCborValue::Array
								   << true
								   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("pointf.untagged") << static_cast<int>(QMetaType::QPointF)
									 << static_cast<QCborTag>(CborSerializer::NoTag)
									 << QCborValue::Array
									 << true
									 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("point.guessed") << static_cast<int>(QMetaType::UnknownType)
								   << static_cast<QCborTag>(CborSerializer::GeomPoint)
								   << QCborValue::Array
								   << false
								   << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("point.invalid.data") << static_cast<int>(QMetaType::QPoint)
										<< static_cast<QCborTag>(CborSerializer::GeomPoint)
										<< QCborValue::Map
										<< true
										<< TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("point.invalid.tag") << static_cast<int>(QMetaType::QPointF)
									   << static_cast<QCborTag>(CborSerializer::GeomSize)
									   << QCborValue::Map
									   << true
									   << TypeConverter::DeserializationCapabilityResult::WrongTag;

	QTest::newRow("line.tagged") << static_cast<int>(QMetaType::QLine)
								 << static_cast<QCborTag>(CborSerializer::GeomLine)
								 << QCborValue::Array
								 << true
								 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("line.untagged") << static_cast<int>(QMetaType::QLine)
								   << static_cast<QCborTag>(CborSerializer::NoTag)
								   << QCborValue::Array
								   << true
								   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("linef.tagged") << static_cast<int>(QMetaType::QLineF)
								  << static_cast<QCborTag>(CborSerializer::GeomLine)
								  << QCborValue::Array
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("linef.untagged") << static_cast<int>(QMetaType::QLine)
									<< static_cast<QCborTag>(CborSerializer::NoTag)
									<< QCborValue::Array
									<< true
									<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("line.guessed") << static_cast<int>(QMetaType::UnknownType)
								  << static_cast<QCborTag>(CborSerializer::GeomLine)
								  << QCborValue::Array
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("line.invalid.data") << static_cast<int>(QMetaType::QLine)
									   << static_cast<QCborTag>(CborSerializer::GeomLine)
									   << QCborValue::Map
									   << true
									   << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("line.invalid.tag") << static_cast<int>(QMetaType::QLineF)
									  << static_cast<QCborTag>(CborSerializer::GeomRect)
									  << QCborValue::Map
									  << true
									  << TypeConverter::DeserializationCapabilityResult::WrongTag;

	QTest::newRow("rect.tagged") << static_cast<int>(QMetaType::QRect)
								 << static_cast<QCborTag>(CborSerializer::GeomRect)
								 << QCborValue::Array
								 << true
								 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("rect.untagged") << static_cast<int>(QMetaType::QRect)
								   << static_cast<QCborTag>(CborSerializer::NoTag)
								   << QCborValue::Array
								   << true
								   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("rectf.tagged") << static_cast<int>(QMetaType::QRectF)
								  << static_cast<QCborTag>(CborSerializer::GeomRect)
								  << QCborValue::Array
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("rectf.untagged") << static_cast<int>(QMetaType::QRectF)
									<< static_cast<QCborTag>(CborSerializer::NoTag)
									<< QCborValue::Array
									<< true
									<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("rect.guessed") << static_cast<int>(QMetaType::UnknownType)
								  << static_cast<QCborTag>(CborSerializer::GeomRect)
								  << QCborValue::Array
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("rect.invalid.data") << static_cast<int>(QMetaType::QRect)
									   << static_cast<QCborTag>(CborSerializer::GeomRect)
									   << QCborValue::Map
									   << true
									   << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("rect.invalid.tag") << static_cast<int>(QMetaType::QRectF)
									  << static_cast<QCborTag>(CborSerializer::GeomLine)
									  << QCborValue::Map
									  << true
									  << TypeConverter::DeserializationCapabilityResult::WrongTag;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QMatrix)
								  << static_cast<QCborTag>(CborSerializer::NoTag)
								  << QCborValue::Array
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
#else
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QTransform)
								  << static_cast<QCborTag>(CborSerializer::NoTag)
								  << QCborValue::Array
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
#endif
}

void GeomConverterTest::addCommonSerData()
{
	QTest::newRow("size.int") << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QSize)
							  << QVariant{QSize{10, 20}}
							  << QCborValue{
									 static_cast<QCborTag>(CborSerializer::GeomSize),
									 QCborArray{10, 20}
								 }
							  << QJsonValue{QJsonArray{10, 20}};
	QTest::newRow("size.real") << QVariantHash{}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QSizeF)
							   << QVariant{QSizeF{10.1, 20.2}}
							   << QCborValue{
									  static_cast<QCborTag>(CborSerializer::GeomSize),
									  QCborArray{10.1, 20.2}
								  }
							   << QJsonValue{QJsonArray{10.1, 20.2}};

	QTest::newRow("point.int") << QVariantHash{}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QPoint)
							   << QVariant{QPoint{10, 20}}
							   << QCborValue{
									  static_cast<QCborTag>(CborSerializer::GeomPoint),
									  QCborArray{10, 20}
								  }
							   << QJsonValue{QJsonArray{10, 20}};
	QTest::newRow("point.real") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QPointF)
								<< QVariant{QPointF{10.1, 20.2}}
								<< QCborValue{
									   static_cast<QCborTag>(CborSerializer::GeomPoint),
									   QCborArray{10.1, 20.2}
								   }
								<< QJsonValue{QJsonArray{10.1, 20.2}};

	QTest::newRow("line.int") << QVariantHash{}
							  << TestQ{
										{QMetaType::QPoint, QVariant{QPoint{10, 10}}, 24},
										{QMetaType::QPoint, QVariant{QPoint{20, 20}}, 42}
									}
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QLine)
							  << QVariant{QLine{QPoint{10, 10}, QPoint{20, 20}}}
							  << QCborValue{
									 static_cast<QCborTag>(CborSerializer::GeomLine),
									 QCborArray{24, 42}
								 }
							  << QJsonValue{QJsonArray{24, 42}};
	QTest::newRow("line.real") << QVariantHash{}
							   << TestQ{
									  {QMetaType::QPointF, QVariant{QPointF{10.1, 10.1}}, 2.4},
									  {QMetaType::QPointF, QVariant{QPointF{20.2, 20.2}}, 4.2}
								  }
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QLineF)
							   << QVariant{QLineF{QPointF{10.1, 10.1}, QPointF{20.2, 20.2}}}
							   << QCborValue{
									  static_cast<QCborTag>(CborSerializer::GeomLine),
									  QCborArray{2.4, 4.2}
								  }
							   << QJsonValue{QJsonArray{2.4, 4.2}};

	QTest::newRow("rect.int") << QVariantHash{}
							  << TestQ{
										{QMetaType::QPoint, QVariant{QPoint{10, 10}}, 24},
										{QMetaType::QSize, QVariant{QSize{20, 20}}, 42}
									}
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QRect)
							  << QVariant{QRect{10, 10, 20, 20}}
							  << QCborValue{
									 static_cast<QCborTag>(CborSerializer::GeomRect),
									 QCborArray{24, 42}
								 }
							  << QJsonValue{QJsonArray{24, 42}};
	QTest::newRow("rect.real") << QVariantHash{}
							   << TestQ{
									  {QMetaType::QPointF, QVariant{QPointF{10.1, 10.1}}, 2.4},
									  {QMetaType::QSizeF, QVariant{QSizeF{20.2, 20.2}}, 4.2}
								  }
							   << static_cast<QObject*>(nullptr)
							   << static_cast<int>(QMetaType::QRectF)
							   << QVariant{QRectF{10.1, 10.1, 20.2, 20.2}}
							   << QCborValue{
									  static_cast<QCborTag>(CborSerializer::GeomRect),
									  QCborArray{2.4, 4.2}
								  }
							   << QJsonValue{QJsonArray{2.4, 4.2}};
}

void GeomConverterTest::addDeserData()
{
	QTest::newRow("size.invalid.data") << QVariantHash{}
									   << TestQ{}
									   << static_cast<QObject*>(nullptr)
									   << static_cast<int>(QMetaType::QSize)
									   << QVariant{}
									   << QCborValue{
											  static_cast<QCborTag>(CborSerializer::GeomSize),
											  QCborArray{11, 2.2}
										  }
									   << QJsonValue{QJsonArray{11, 2.2}};
	QTest::newRow("size.invalid.array") << QVariantHash{}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QSizeF)
										<< QVariant{}
										<< QCborValue{
											   static_cast<QCborTag>(CborSerializer::GeomSize),
											   QCborArray{11, 22, 33}
										   }
										<< QJsonValue{QJsonArray{11, 22, 33}};

	QTest::newRow("point.invalid.data") << QVariantHash{}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QPoint)
										<< QVariant{}
										<< QCborValue{
											   static_cast<QCborTag>(CborSerializer::GeomPoint),
											   QCborArray{11, 2.2}
										   }
										<< QJsonValue{QJsonArray{11, 2.2}};
	QTest::newRow("point.invalid.array") << QVariantHash{}
										 << TestQ{}
										 << static_cast<QObject*>(nullptr)
										 << static_cast<int>(QMetaType::QPointF)
										 << QVariant{}
										 << QCborValue{
												static_cast<QCborTag>(CborSerializer::GeomPoint),
												QCborArray{11, 22, 33}
											}
										 << QJsonValue{QJsonArray{11, 22, 33}};

	QTest::newRow("line.invalid.data") << QVariantHash{}
									   << TestQ{}
									   << static_cast<QObject*>(nullptr)
									   << static_cast<int>(QMetaType::QLine)
									   << QVariant{}
									   << QCborValue{
											  static_cast<QCborTag>(CborSerializer::GeomLine),
											  QCborArray{11, 2.2}
										  }
									   << QJsonValue{QJsonArray{11, 2.2}};
	QTest::newRow("line.invalid.array") << QVariantHash{}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QLineF)
										<< QVariant{}
										<< QCborValue{
											   static_cast<QCborTag>(CborSerializer::GeomLine),
											   QCborArray{11, 22, 33}
										   }
										<< QJsonValue{QJsonArray{11, 22, 33}};

	QTest::newRow("rect.invalid.data") << QVariantHash{}
									   << TestQ{}
									   << static_cast<QObject*>(nullptr)
									   << static_cast<int>(QMetaType::QRect)
									   << QVariant{}
									   << QCborValue{
											  static_cast<QCborTag>(CborSerializer::GeomRect),
											  QCborArray{11, 2.2}
										  }
									   << QJsonValue{QJsonArray{11, 2.2}};
	QTest::newRow("rect.invalid.array") << QVariantHash{}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QRectF)
										<< QVariant{}
										<< QCborValue{
											   static_cast<QCborTag>(CborSerializer::GeomRect),
											   QCborArray{11, 22, 33}
										   }
										<< QJsonValue{QJsonArray{11, 22, 33}};
}

QTEST_MAIN(GeomConverterTest)

#include "tst_geomconverter.moc"
