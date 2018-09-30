#include <QtTest>
#include <QtJsonSerializer>

#include "multitypeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonjsonconverter_p.h>

class JsonConverterTest : public MultiTypeConverterTestBase
{
	Q_OBJECT

protected:
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;

private:
	QJsonJsonValueConverter _jValConverter;
	QJsonJsonObjectConverter _jObjConverter;
	QJsonJsonArrayConverter _jArrConverter;

	QJsonTypeConverter * const jValConverter = &_jValConverter;
	QJsonTypeConverter * const jObjConverter = &_jObjConverter;
	QJsonTypeConverter * const jArrConverter = &_jArrConverter;
};

void JsonConverterTest::addConverterData()
{
	QTest::newRow("json.value") << jValConverter
								<< static_cast<int>(QJsonTypeConverter::Standard)
								<< QList<QJsonValue::Type>{QJsonValue::Null, QJsonValue::Bool, QJsonValue::Double, QJsonValue::String, QJsonValue::Array, QJsonValue::Object};

	QTest::newRow("json.object") << jObjConverter
								 << static_cast<int>(QJsonTypeConverter::Standard)
								 << QList<QJsonValue::Type>{QJsonValue::Object};

	QTest::newRow("json.array") << jArrConverter
								<< static_cast<int>(QJsonTypeConverter::Standard)
								<< QList<QJsonValue::Type>{QJsonValue::Array};
}

void JsonConverterTest::addMetaData()
{
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
}

void JsonConverterTest::addCommonSerData()
{
	QTest::newRow("json.value") << jValConverter
								<< QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QJsonValue)
								<< QVariant{QJsonValue{42}}
								<< QJsonValue{42};

	QTest::newRow("json.object") << jObjConverter
								 << QVariantHash{}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << static_cast<int>(QMetaType::QJsonObject)
								 << QVariant{QJsonObject{{QStringLiteral("baum"), 42}}}
								 << QJsonValue{QJsonObject{{QStringLiteral("baum"), 42}}};

	QTest::newRow("json.array") << jArrConverter
								<< QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QJsonArray)
								<< QVariant{QJsonArray{10, 20, true, 4.2}}
								<< QJsonValue{QJsonArray{10, 20, true, 4.2}};
}

QTEST_MAIN(JsonConverterTest)

#include "tst_jsonconverter.moc"
