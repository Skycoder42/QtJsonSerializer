#include <QtTest>
#include <QtJsonSerializer>

#include "multitypeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonjsonconverter_p.h>

class JsonConverterTest : public MultiTypeConverterTestBase
{
	Q_OBJECT

protected:
	void addConverterInstances() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;

private:
	QJsonJsonValueConverter _jValConverter;
	QJsonJsonObjectConverter _jObjConverter;
	QJsonJsonArrayConverter _jArrConverter;

	QJsonTypeConverter *jValConverter = &_jValConverter;
	QJsonTypeConverter *jObjConverter = &_jObjConverter;
	QJsonTypeConverter *jArrConverter = &_jArrConverter;
};

void JsonConverterTest::addConverterInstances()
{
	QTest::newRow("value") << jValConverter;
	QTest::newRow("object") << jObjConverter;
	QTest::newRow("array") << jArrConverter;
}

void JsonConverterTest::addConverterData()
{
	QTest::newRow("value") << jValConverter
						   << static_cast<int>(QJsonTypeConverter::Standard)
						   << QList<QJsonValue::Type>{QJsonValue::Null, QJsonValue::Bool, QJsonValue::Double, QJsonValue::String, QJsonValue::Array, QJsonValue::Object};

	QTest::newRow("object") << jObjConverter
							<< static_cast<int>(QJsonTypeConverter::Standard)
							<< QList<QJsonValue::Type>{QJsonValue::Object};

	QTest::newRow("array") << jArrConverter
						   << static_cast<int>(QJsonTypeConverter::Standard)
						   << QList<QJsonValue::Type>{QJsonValue::Array};
}

void JsonConverterTest::addMetaData()
{
	QTest::newRow("value.value") << jValConverter
								 << static_cast<int>(QMetaType::QJsonValue)
								 << true;
	QTest::newRow("value.invalid") << jValConverter
								   << static_cast<int>(QMetaType::QJsonObject)
								   << false;

	QTest::newRow("object.object") << jObjConverter
								   << static_cast<int>(QMetaType::QJsonObject)
								   << true;
	QTest::newRow("object.invalid") << jObjConverter
									<< static_cast<int>(QMetaType::QJsonArray)
									<< false;

	QTest::newRow("array.array") << jArrConverter
								 << static_cast<int>(QMetaType::QJsonArray)
								 << true;
	QTest::newRow("array.invalid") << jArrConverter
								   << static_cast<int>(QMetaType::QJsonObject)
								   << false;
}

void JsonConverterTest::addCommonSerData()
{
	QTest::newRow("value") << jValConverter
						   << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << static_cast<int>(QMetaType::QJsonValue)
						   << QVariant{QJsonValue{42}}
						   << QJsonValue{42};

	QTest::newRow("object") << jObjConverter
							<< QVariantHash{}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< static_cast<int>(QMetaType::QJsonObject)
							<< QVariant{QJsonObject{{QStringLiteral("baum"), 42}}}
							<< QJsonValue{QJsonObject{{QStringLiteral("baum"), 42}}};

	QTest::newRow("array") << jArrConverter
						   << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << static_cast<int>(QMetaType::QJsonArray)
						   << QVariant{QJsonArray{10, 20, true, 4.2}}
						   << QJsonValue{QJsonArray{10, 20, true, 4.2}};
}

QTEST_MAIN(JsonConverterTest)

#include "tst_jsonconverter.moc"
