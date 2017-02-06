#include <QString>
#include <QtTest>

class SerializerTest : public QObject
{
	Q_OBJECT

public:
	SerializerTest();

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void testObjectSerialization_data();
	void testObjectSerialization();
};

SerializerTest::SerializerTest()
{
}

void SerializerTest::initTestCase()
{
}

void SerializerTest::cleanupTestCase()
{
}

void SerializerTest::testObjectSerialization_data()
{
	QTest::addColumn<QString>("data");
	QTest::newRow("0") << QString();
}

void SerializerTest::testObjectSerialization()
{
	QFETCH(QString, data);
	QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(SerializerTest)

#include "tst_serializer.moc"
