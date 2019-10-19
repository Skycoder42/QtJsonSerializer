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
	QTest::newRow("chrono::duration") << static_cast<int>(QJsonTypeConverter::Standard);
}

void ChronoDurationConverterTest::addMetaData()
{
	QTest::newRow("nano.tagged.nano") << qMetaTypeId<nanoseconds>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("nano.tagged.micro") << qMetaTypeId<nanoseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("nano.tagged.milli") << qMetaTypeId<nanoseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("nano.tagged.secs") << qMetaTypeId<nanoseconds>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoSeconds)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("nano.tagged.mins") << qMetaTypeId<nanoseconds>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoMinutes)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("nano.tagged.hours") << qMetaTypeId<nanoseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoHours)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("nano.untagged") << qMetaTypeId<nanoseconds>()
								   << static_cast<QCborTag>(QJsonTypeConverter::NoTag)
								   << QCborValue::Double
								   << true
								   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("nano.guessed") << static_cast<int>(QMetaType::UnknownType)
								  << static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds)
								  << QCborValue::Integer
								  << false
								  << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;

	QTest::newRow("micro.tagged.nano") << qMetaTypeId<microseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("micro.tagged.micro") << qMetaTypeId<microseconds>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("micro.tagged.milli") << qMetaTypeId<microseconds>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("micro.tagged.secs") << qMetaTypeId<microseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("micro.tagged.mins") << qMetaTypeId<microseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMinutes)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("micro.tagged.hours") << qMetaTypeId<microseconds>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoHours)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("micro.untagged") << qMetaTypeId<microseconds>()
									<< static_cast<QCborTag>(QJsonTypeConverter::NoTag)
									<< QCborValue::Double
									<< true
									<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("micro.guessed") << static_cast<int>(QMetaType::UnknownType)
								   << static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds)
								   << QCborValue::Integer
								   << false
								   << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;

	QTest::newRow("milli.tagged.nano") << qMetaTypeId<milliseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("milli.tagged.micro") << qMetaTypeId<milliseconds>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("milli.tagged.milli") << qMetaTypeId<milliseconds>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("milli.tagged.secs") << qMetaTypeId<milliseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("milli.tagged.mins") << qMetaTypeId<milliseconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMinutes)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("milli.tagged.hours") << qMetaTypeId<milliseconds>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoHours)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("milli.untagged") << qMetaTypeId<milliseconds>()
									<< static_cast<QCborTag>(QJsonTypeConverter::NoTag)
									<< QCborValue::Double
									<< true
									<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("milli.guessed") << static_cast<int>(QMetaType::UnknownType)
								   << static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds)
								   << QCborValue::Integer
								   << false
								   << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;

	QTest::newRow("secs.tagged.nano") << qMetaTypeId<seconds>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("secs.tagged.micro") << qMetaTypeId<seconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("secs.tagged.milli") << qMetaTypeId<seconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("secs.tagged.secs") << qMetaTypeId<seconds>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoSeconds)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("secs.tagged.mins") << qMetaTypeId<seconds>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoMinutes)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("secs.tagged.hours") << qMetaTypeId<seconds>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoHours)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("secs.untagged") << qMetaTypeId<seconds>()
								   << static_cast<QCborTag>(QJsonTypeConverter::NoTag)
								   << QCborValue::Double
								   << true
								   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("secs.guessed") << static_cast<int>(QMetaType::UnknownType)
								  << static_cast<QCborTag>(QCborSerializer::ChronoSeconds)
								  << QCborValue::Integer
								  << false
								  << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;

	QTest::newRow("mins.tagged.nano") << qMetaTypeId<minutes>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("mins.tagged.micro") << qMetaTypeId<minutes>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("mins.tagged.milli") << qMetaTypeId<minutes>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("mins.tagged.secs") << qMetaTypeId<minutes>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoSeconds)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("mins.tagged.mins") << qMetaTypeId<minutes>()
									  << static_cast<QCborTag>(QCborSerializer::ChronoMinutes)
									  << QCborValue::Integer
									  << true
									  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("mins.tagged.hours") << qMetaTypeId<minutes>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoHours)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("mins.untagged") << qMetaTypeId<minutes>()
								   << static_cast<QCborTag>(QJsonTypeConverter::NoTag)
								   << QCborValue::Double
								   << true
								   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("mins.guessed") << static_cast<int>(QMetaType::UnknownType)
								  << static_cast<QCborTag>(QCborSerializer::ChronoMinutes)
								  << QCborValue::Integer
								  << false
								  << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;

	QTest::newRow("hours.tagged.nano") << qMetaTypeId<hours>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("hours.tagged.micro") << qMetaTypeId<hours>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("hours.tagged.milli") << qMetaTypeId<hours>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("hours.tagged.secs") << qMetaTypeId<hours>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoSeconds)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("hours.tagged.mins") << qMetaTypeId<hours>()
									   << static_cast<QCborTag>(QCborSerializer::ChronoMinutes)
									   << QCborValue::Integer
									   << true
									   << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("hours.tagged.hours") << qMetaTypeId<hours>()
										<< static_cast<QCborTag>(QCborSerializer::ChronoHours)
										<< QCborValue::Integer
										<< true
										<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("hours.untagged") << qMetaTypeId<hours>()
									<< static_cast<QCborTag>(QJsonTypeConverter::NoTag)
									<< QCborValue::Double
									<< true
									<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("hours.guessed") << static_cast<int>(QMetaType::UnknownType)
								   << static_cast<QCborTag>(QCborSerializer::ChronoHours)
								   << QCborValue::Integer
								   << false
								   << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;
}

void ChronoDurationConverterTest::addCommonSerData()
{
	QTest::newRow("nanoseconds") << QVariantHash{}
								 << TestQ {}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<nanoseconds>()
								 << QVariant::fromValue(10ns)
								 << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds), 10}
								 << QJsonValue{10};
	QTest::newRow("microseconds") << QVariantHash{}
								  << TestQ {}
								  << static_cast<QObject*>(this)
								  << qMetaTypeId<microseconds>()
								  << QVariant::fromValue(20us)
								  << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds), 20}
								  << QJsonValue{20};
	QTest::newRow("milliseconds") << QVariantHash{}
								  << TestQ {}
								  << static_cast<QObject*>(this)
								  << qMetaTypeId<milliseconds>()
								  << QVariant::fromValue(30ms)
								  << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds), 30}
								  << QJsonValue{30};
	QTest::newRow("seconds") << QVariantHash{}
							 << TestQ {}
							 << static_cast<QObject*>(this)
							 << qMetaTypeId<seconds>()
							 << QVariant::fromValue(40s)
							 << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoSeconds), 40}
							 << QJsonValue{40};
	QTest::newRow("minutes") << QVariantHash{}
							 << TestQ {}
							 << static_cast<QObject*>(this)
							 << qMetaTypeId<minutes>()
							 << QVariant::fromValue(50min)
							 << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMinutes), 50}
							 << QJsonValue{50};
	QTest::newRow("hours") << QVariantHash{}
						   << TestQ {}
						   << static_cast<QObject*>(this)
						   << qMetaTypeId<hours>()
						   << QVariant::fromValue(60h)
						   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoHours), 60}
						   << QJsonValue{60};
}

QTEST_MAIN(ChronoDurationConverterTest)

#include "tst_chronodurationconverter.moc"

