#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonversionnumberconverter_p.h>

class VersionConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	QJsonVersionNumberConverter _converter;
};

void VersionConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<QVersionNumber>();
}

QJsonTypeConverter *VersionConverterTest::converter()
{
	return &_converter;
}

void VersionConverterTest::addConverterData()
{
	QTest::newRow("version") << static_cast<int>(QJsonTypeConverter::Standard)
							 << QList<QJsonValue::Type>{QJsonValue::String};
}

void VersionConverterTest::addMetaData()
{
	QTest::newRow("version") << qMetaTypeId<QVersionNumber>()
							 << true;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QString)
							 << false;
}

void VersionConverterTest::addCommonSerData()
{
	QTest::newRow("simple") << QVariantHash{}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< qMetaTypeId<QVersionNumber>()
							<< QVariant::fromValue(QVersionNumber{1, 2, 3})
							<< QJsonValue{QStringLiteral("1.2.3")};
	QTest::newRow("long") << QVariantHash{}
						  << TestQ{}
						  << static_cast<QObject*>(nullptr)
						  << qMetaTypeId<QVersionNumber>()
						  << QVariant::fromValue(QVersionNumber{1, 2, 3, 4, 5})
						  << QJsonValue{QStringLiteral("1.2.3.4.5")};
}

void VersionConverterTest::addDeserData()
{
	QTest::newRow("suffixed") << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << qMetaTypeId<QVersionNumber>()
							  << QVariant::fromValue(QVersionNumber{1, 2, 3})
							  << QJsonValue{QStringLiteral("1.2.3-r1")};
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << qMetaTypeId<QVersionNumber>()
							 << QVariant{}
							 << QJsonValue{QStringLiteral("A1.4.5")};
}

QTEST_MAIN(VersionConverterTest)

#include "tst_versionconverter.moc"
