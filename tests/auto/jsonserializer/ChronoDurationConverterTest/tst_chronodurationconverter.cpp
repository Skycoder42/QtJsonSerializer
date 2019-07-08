#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonchronodurationconverter_p.h>

#include <chrono>
using namespace std::chrono;
using namespace std::chrono_literals;

Q_DECLARE_METATYPE(std::chrono::nanoseconds)
Q_DECLARE_METATYPE(std::chrono::microseconds)
Q_DECLARE_METATYPE(std::chrono::milliseconds)
Q_DECLARE_METATYPE(std::chrono::seconds)
Q_DECLARE_METATYPE(std::chrono::minutes)
Q_DECLARE_METATYPE(std::chrono::hours)

class ChronoDurationConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;

private:
	QJsonChronoDurationConverter _converter;
};

void ChronoDurationConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<nanoseconds>();
	QMetaType::registerEqualsComparator<microseconds>();
	QMetaType::registerEqualsComparator<milliseconds>();
	QMetaType::registerEqualsComparator<seconds>();
	QMetaType::registerEqualsComparator<minutes>();
	QMetaType::registerEqualsComparator<hours>();
}

QJsonTypeConverter *ChronoDurationConverterTest::converter()
{
	return &_converter;
}

void ChronoDurationConverterTest::addConverterData()
{
	QTest::newRow("chrono::duration") << static_cast<int>(QJsonTypeConverter::Standard)
									  << QList<QJsonValue::Type>{QJsonValue::Double};
}

void ChronoDurationConverterTest::addMetaData()
{
	QTest::newRow("nanoseconds") << qMetaTypeId<nanoseconds>()
								 << true;
	QTest::newRow("microseconds") << qMetaTypeId<microseconds>()
								  << true;
	QTest::newRow("milliseconds") << qMetaTypeId<milliseconds>()
								  << true;
	QTest::newRow("seconds") << qMetaTypeId<seconds>()
							 << true;
	QTest::newRow("minutes") << qMetaTypeId<minutes>()
							 << true;
	QTest::newRow("hours") << qMetaTypeId<hours>()
						   << true;
	QTest::newRow("invalid") << qMetaTypeId<qint64>()
							 << false;
}

void ChronoDurationConverterTest::addCommonSerData()
{
	QTest::newRow("nanoseconds") << QVariantHash{}
								 << TestQ {}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<nanoseconds>()
								 << QVariant::fromValue(10ns)
								 << QJsonValue{10};
	QTest::newRow("microseconds") << QVariantHash{}
								  << TestQ {}
								  << static_cast<QObject*>(this)
								  << qMetaTypeId<microseconds>()
								  << QVariant::fromValue(20us)
								  << QJsonValue{20};
	QTest::newRow("milliseconds") << QVariantHash{}
								  << TestQ {}
								  << static_cast<QObject*>(this)
								  << qMetaTypeId<milliseconds>()
								  << QVariant::fromValue(30ms)
								  << QJsonValue{30};
	QTest::newRow("seconds") << QVariantHash{}
							 << TestQ {}
							 << static_cast<QObject*>(this)
							 << qMetaTypeId<seconds>()
							 << QVariant::fromValue(40s)
							 << QJsonValue{40};
	QTest::newRow("minutes") << QVariantHash{}
							 << TestQ {}
							 << static_cast<QObject*>(this)
							 << qMetaTypeId<minutes>()
							 << QVariant::fromValue(50min)
							 << QJsonValue{50};
	QTest::newRow("hours") << QVariantHash{}
						   << TestQ {}
						   << static_cast<QObject*>(this)
						   << qMetaTypeId<hours>()
						   << QVariant::fromValue(60h)
						   << QJsonValue{60};
}

QTEST_MAIN(ChronoDurationConverterTest)

#include "tst_chronodurationconverter.moc"

