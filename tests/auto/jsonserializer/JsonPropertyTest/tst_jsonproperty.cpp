#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QtJsonSerializer>

#include "jsongadget.h"

class JsonPropertyTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testSerialization_data();
	void testSerialization();

	void testDeserialization_data();
	void testDeserialization();

private:
	QJsonSerializer *serializer;

	void generateTestData();
};

void JsonPropertyTest::initTestCase()
{
#ifdef Q_OS_LINUX
	Q_ASSERT(qgetenv("LD_PRELOAD").contains("Qt5JsonSerializer"));
#endif

	serializer = new QJsonSerializer(this);
}

void JsonPropertyTest::cleanupTestCase()
{
	serializer->deleteLater();
}

void JsonPropertyTest::testSerialization_data()
{
	QTest::addColumn<JsonGadget>("data");
	QTest::addColumn<QJsonObject>("result");

	generateTestData();
}

void JsonPropertyTest::testSerialization()
{
	QFETCH(JsonGadget, data);
	QFETCH(QJsonObject, result);

	try {
		auto res = serializer->serialize(data);
		QCOMPARE(res, result);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void JsonPropertyTest::testDeserialization_data()
{
	QTest::addColumn<JsonGadget>("result");
	QTest::addColumn<QJsonObject>("data");

	generateTestData();
}

void JsonPropertyTest::testDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(JsonGadget, result);

	try {
		auto res = serializer->deserialize<JsonGadget>(data);
		QCOMPARE(res, result);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void JsonPropertyTest::generateTestData()
{
	QTest::newRow("default") << JsonGadget()
							 << QJsonObject({
												{"object", QJsonObject()},
												{"array", QJsonArray()},
												{"value", QJsonValue::Null}
											});

	QJsonObject obj;
	obj["id"] = 42;
	obj["text"] = "baum";
	QTest::newRow("object") << JsonGadget(obj)
							<< QJsonObject({
											   {"object", obj},
											   {"array", QJsonArray()},
											   {"value", QJsonValue::Null}
										   });

	QJsonArray arr;
	arr.append(42);
	arr.append("baum");
	arr.append(obj);
	QTest::newRow("array") << JsonGadget(arr)
						   << QJsonObject({
											  {"object", QJsonObject()},
											  {"array", arr},
											  {"value", QJsonValue::Null}
										  });

	QTest::newRow("objectValue") << JsonGadget(QJsonValue(obj))
								 << QJsonObject({
													{"object", QJsonObject()},
													{"array", QJsonArray()},
													{"value", obj}
												});

	QTest::newRow("arrayValue") << JsonGadget(QJsonValue(arr))
								 << QJsonObject({
													{"object", QJsonObject()},
													{"array", QJsonArray()},
													{"value", arr}
												});
}

QTEST_MAIN(JsonPropertyTest)

#include "tst_jsonproperty.moc"
