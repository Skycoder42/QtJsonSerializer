#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsoncborconverter_p.h>

class CborConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;

private:
	QJsonCborConverter _converter;
};

QJsonTypeConverter *CborConverterTest::converter()
{
	return &_converter;
}

void CborConverterTest::addConverterData()
{
	QTest::newRow("cbor") << static_cast<int>(QJsonTypeConverter::Standard);
}

void CborConverterTest::addMetaData()
{
	QTest::newRow("cbor.value") << static_cast<int>(QMetaType::QCborValue)
								<< static_cast<QCborTag>(QCborSerializer::NoTag)
								<< QCborValue::Double
								<< true
								<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("cbor.map") << static_cast<int>(QMetaType::QCborMap)
							  << static_cast<QCborTag>(QCborSerializer::NoTag)
							  << QCborValue::Map
							  << true
							  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("cbor.map.invalid") << static_cast<int>(QMetaType::QCborMap)
									  << static_cast<QCborTag>(QCborSerializer::NoTag)
									  << QCborValue::Array
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("cbor.array") << static_cast<int>(QMetaType::QCborArray)
								<< static_cast<QCborTag>(QCborSerializer::NoTag)
								<< QCborValue::Array
								<< true
								<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("cbor.array.invalid") << static_cast<int>(QMetaType::QCborArray)
										<< static_cast<QCborTag>(QCborSerializer::NoTag)
										<< QCborValue::Double
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("cbor.simple") << static_cast<int>(QMetaType::QCborSimpleType)
								 << static_cast<QCborTag>(QCborSerializer::NoTag)
								 << QCborValue::False
								 << true
								 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("cbor.simple.invalid") << static_cast<int>(QMetaType::QCborSimpleType)
										 << static_cast<QCborTag>(QCborSerializer::NoTag)
										 << QCborValue::String
										 << true
										 << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("json.value") << static_cast<int>(QMetaType::QJsonValue)
								<< static_cast<QCborTag>(QCborSerializer::NoTag)
								<< QCborValue::Double
								<< true
								<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("json.value.invalid") << static_cast<int>(QMetaType::QJsonValue)
										<< static_cast<QCborTag>(QCborSerializer::NoTag)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("json.obj") << static_cast<int>(QMetaType::QJsonObject)
							  << static_cast<QCborTag>(QCborSerializer::NoTag)
							  << QCborValue::Map
							  << true
							  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("json.obj.invalid") << static_cast<int>(QMetaType::QJsonObject)
									  << static_cast<QCborTag>(QCborSerializer::NoTag)
									  << QCborValue::Array
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("json.array") << static_cast<int>(QMetaType::QJsonArray)
								<< static_cast<QCborTag>(QCborSerializer::NoTag)
								<< QCborValue::Array
								<< true
								<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("json.array.invalid") << static_cast<int>(QMetaType::QJsonArray)
										<< static_cast<QCborTag>(QCborSerializer::NoTag)
										<< QCborValue::Double
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("json.doc.obj") << static_cast<int>(QMetaType::QJsonDocument)
								  << static_cast<QCborTag>(QCborSerializer::NoTag)
								  << QCborValue::Map
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("json.doc.array") << static_cast<int>(QMetaType::QJsonDocument)
									<< static_cast<QCborTag>(QCborSerializer::NoTag)
									<< QCborValue::Array
									<< true
									<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("json.doc.null") << static_cast<int>(QMetaType::QJsonDocument)
								   << static_cast<QCborTag>(QCborSerializer::NoTag)
								   << QCborValue::Null
								   << true
								   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("json.doc.invalid") << static_cast<int>(QMetaType::QJsonDocument)
									  << static_cast<QCborTag>(QCborSerializer::NoTag)
									  << QCborValue::String
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
}

void CborConverterTest::addCommonSerData()
{
	QTest::newRow("cbor.value") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QCborValue)
								<< QVariant::fromValue(QCborValue{42})
								<< QCborValue{42}
								<< QJsonValue{42};

	QTest::newRow("cbor.map") << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QCborMap)
							  << QVariant::fromValue(QCborMap{{QStringLiteral("baum"), 42}})
							  << QCborValue{QCborMap{{QStringLiteral("baum"), 42}}}
							  << QJsonValue{QJsonObject{{QStringLiteral("baum"), 42}}};

	QTest::newRow("cbor.array") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QCborArray)
								<< QVariant::fromValue(QCborArray{10, 20, true, 4.2})
								<< QCborValue{QCborArray{10, 20, true, 4.2}}
								<< QJsonValue{QJsonArray{10, 20, true, 4.2}};

	QTest::newRow("cbor.simple") << QVariantHash{}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << static_cast<int>(QMetaType::QCborSimpleType)
								 << QVariant::fromValue(QCborSimpleType::False)
								 << QCborValue{false}
								 << QJsonValue{false};

	QTest::newRow("json.value") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QJsonValue)
								<< QVariant{QJsonValue{42}}
								<< QCborValue{42}
								<< QJsonValue{42};

	QTest::newRow("json.obj") << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << static_cast<int>(QMetaType::QJsonObject)
							  << QVariant{QJsonObject{{QStringLiteral("baum"), 42}}}
							  << QCborValue{QCborMap{{QStringLiteral("baum"), 42}}}
							  << QJsonValue{QJsonObject{{QStringLiteral("baum"), 42}}};

	QTest::newRow("json.array") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QJsonArray)
								<< QVariant{QJsonArray{10, 20, true, 4.2}}
								<< QCborValue{QCborArray{10, 20, true, 4.2}}
								<< QJsonValue{QJsonArray{10, 20, true, 4.2}};

	QTest::newRow("json.doc.obj") << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QJsonDocument)
								  << QVariant{QJsonDocument{QJsonObject{{QStringLiteral("baum"), 42}}}}
								  << QCborValue{QCborMap{{QStringLiteral("baum"), 42}}}
								  << QJsonValue{QJsonObject{{QStringLiteral("baum"), 42}}};

	QTest::newRow("json.doc.array") << QVariantHash{}
									<< TestQ{}
									<< static_cast<QObject*>(nullptr)
									<< static_cast<int>(QMetaType::QJsonDocument)
									<< QVariant{QJsonDocument{QJsonArray{10, 20, true, 4.2}}}
									<< QCborValue{QCborArray{10, 20, true, 4.2}}
									<< QJsonValue{QJsonArray{10, 20, true, 4.2}};
}

QTEST_MAIN(CborConverterTest)

#include "tst_cborconverter.moc"
