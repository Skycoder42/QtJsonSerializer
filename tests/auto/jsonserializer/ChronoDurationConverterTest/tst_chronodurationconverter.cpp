#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonchronodurationconverter_p.h>

#include <chrono>
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

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
	void addDeserData() override;

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
								   << static_cast<QCborTag>(QCborSerializer::NoTag)
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
									<< static_cast<QCborTag>(QCborSerializer::NoTag)
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
									<< static_cast<QCborTag>(QCborSerializer::NoTag)
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
								   << static_cast<QCborTag>(QCborSerializer::NoTag)
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
								   << static_cast<QCborTag>(QCborSerializer::NoTag)
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
									<< static_cast<QCborTag>(QCborSerializer::NoTag)
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

void ChronoDurationConverterTest::addDeserData()
{
	QTest::newRow("nanoseconds.micro") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<nanoseconds>()
									   << QVariant::fromValue(duration_cast<nanoseconds>(20us))
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("nanoseconds.milli") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<nanoseconds>()
									   << QVariant::fromValue(duration_cast<nanoseconds>(20ms))
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("nanoseconds.secs") << QVariantHash{}
									  << TestQ {}
									  << static_cast<QObject*>(this)
									  << qMetaTypeId<nanoseconds>()
									  << QVariant::fromValue(duration_cast<nanoseconds>(20s))
									  << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoSeconds), 20}
									  << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("nanoseconds.mins") << QVariantHash{}
									  << TestQ {}
									  << static_cast<QObject*>(this)
									  << qMetaTypeId<nanoseconds>()
									  << QVariant::fromValue(duration_cast<nanoseconds>(20min))
									  << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMinutes), 20}
									  << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("nanoseconds.hours") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<nanoseconds>()
									   << QVariant::fromValue(duration_cast<nanoseconds>(20h))
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoHours), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("nanoseconds.invalid") << QVariantHash{}
										 << TestQ {}
										 << static_cast<QObject*>(this)
										 << qMetaTypeId<nanoseconds>()
										 << QVariant{}
										 << QCborValue{QCborKnownTags::Signature, 20}
										 << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("microseconds.nano") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<microseconds>()
									   << QVariant{}
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("microseconds.milli") << QVariantHash{}
										<< TestQ {}
										<< static_cast<QObject*>(this)
										<< qMetaTypeId<microseconds>()
										<< QVariant::fromValue(duration_cast<microseconds>(20ms))
										<< QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds), 20}
										<< QJsonValue{QJsonValue::Undefined};
	QTest::newRow("microseconds.secs") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<microseconds>()
									   << QVariant::fromValue(duration_cast<microseconds>(20s))
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoSeconds), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("microseconds.mins") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<microseconds>()
									   << QVariant::fromValue(duration_cast<microseconds>(20min))
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMinutes), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("microseconds.hours") << QVariantHash{}
										<< TestQ {}
										<< static_cast<QObject*>(this)
										<< qMetaTypeId<microseconds>()
										<< QVariant::fromValue(duration_cast<microseconds>(20h))
										<< QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoHours), 20}
										<< QJsonValue{QJsonValue::Undefined};
	QTest::newRow("microseconds.invalid") << QVariantHash{}
										  << TestQ {}
										  << static_cast<QObject*>(this)
										  << qMetaTypeId<microseconds>()
										  << QVariant{}
										  << QCborValue{QCborKnownTags::Signature, 20}
										  << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("milliseconds.nano") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<milliseconds>()
									   << QVariant{}
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("milliseconds.micro") << QVariantHash{}
										<< TestQ {}
										<< static_cast<QObject*>(this)
										<< qMetaTypeId<milliseconds>()
										<< QVariant{}
										<< QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds), 20}
										<< QJsonValue{QJsonValue::Undefined};
	QTest::newRow("milliseconds.secs") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<milliseconds>()
									   << QVariant::fromValue(duration_cast<milliseconds>(20s))
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoSeconds), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("milliseconds.mins") << QVariantHash{}
									   << TestQ {}
									   << static_cast<QObject*>(this)
									   << qMetaTypeId<milliseconds>()
									   << QVariant::fromValue(duration_cast<milliseconds>(20min))
									   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMinutes), 20}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("milliseconds.hours") << QVariantHash{}
										<< TestQ {}
										<< static_cast<QObject*>(this)
										<< qMetaTypeId<milliseconds>()
										<< QVariant::fromValue(duration_cast<milliseconds>(20h))
										<< QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoHours), 20}
										<< QJsonValue{QJsonValue::Undefined};
	QTest::newRow("milliseconds.invalid") << QVariantHash{}
										  << TestQ {}
										  << static_cast<QObject*>(this)
										  << qMetaTypeId<milliseconds>()
										  << QVariant{}
										  << QCborValue{QCborKnownTags::Signature, 20}
										  << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("seconds.nano") << QVariantHash{}
								  << TestQ {}
								  << static_cast<QObject*>(this)
								  << qMetaTypeId<seconds>()
								  << QVariant{}
								  << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds), 20}
								  << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("seconds.micro") << QVariantHash{}
								   << TestQ {}
								   << static_cast<QObject*>(this)
								   << qMetaTypeId<seconds>()
								   << QVariant{}
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds), 20}
								   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("seconds.milli") << QVariantHash{}
								   << TestQ {}
								   << static_cast<QObject*>(this)
								   << qMetaTypeId<seconds>()
								   << QVariant{}
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds), 20}
								   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("seconds.mins") << QVariantHash{}
								  << TestQ {}
								  << static_cast<QObject*>(this)
								  << qMetaTypeId<seconds>()
								  << QVariant::fromValue(duration_cast<seconds>(20min))
								  << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMinutes), 20}
								  << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("seconds.hours") << QVariantHash{}
								   << TestQ {}
								   << static_cast<QObject*>(this)
								   << qMetaTypeId<seconds>()
								   << QVariant::fromValue(duration_cast<seconds>(20h))
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoHours), 20}
								   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("seconds.invalid") << QVariantHash{}
									 << TestQ {}
									 << static_cast<QObject*>(this)
									 << qMetaTypeId<seconds>()
									 << QVariant{}
									 << QCborValue{QCborKnownTags::Signature, 20}
									 << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("minutes.nano") << QVariantHash{}
								  << TestQ {}
								  << static_cast<QObject*>(this)
								  << qMetaTypeId<minutes>()
								  << QVariant{}
								  << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds), 20}
								  << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("minutes.micro") << QVariantHash{}
								   << TestQ {}
								   << static_cast<QObject*>(this)
								   << qMetaTypeId<minutes>()
								   << QVariant{}
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds), 20}
								   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("minutes.milli") << QVariantHash{}
								   << TestQ {}
								   << static_cast<QObject*>(this)
								   << qMetaTypeId<minutes>()
								   << QVariant{}
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds), 20}
								   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("minutes.secs") << QVariantHash{}
								  << TestQ {}
								  << static_cast<QObject*>(this)
								  << qMetaTypeId<minutes>()
								  << QVariant{}
								  << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoSeconds), 20}
								  << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("minutes.hours") << QVariantHash{}
								   << TestQ {}
								   << static_cast<QObject*>(this)
								   << qMetaTypeId<minutes>()
								   << QVariant::fromValue(duration_cast<minutes>(20h))
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoHours), 20}
								   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("minutes.invalid") << QVariantHash{}
									 << TestQ {}
									 << static_cast<QObject*>(this)
									 << qMetaTypeId<minutes>()
									 << QVariant{}
									 << QCborValue{QCborKnownTags::Signature, 20}
									 << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("hours.nano") << QVariantHash{}
								<< TestQ {}
								<< static_cast<QObject*>(this)
								<< qMetaTypeId<hours>()
								<< QVariant{}
								<< QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoNanoSeconds), 20}
								<< QJsonValue{QJsonValue::Undefined};
	QTest::newRow("hours.micro") << QVariantHash{}
								 << TestQ {}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<hours>()
								 << QVariant{}
								 << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMicroSeconds), 20}
								 << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("hours.milli") << QVariantHash{}
								 << TestQ {}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<hours>()
								 << QVariant{}
								 << QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMilliSeconds), 20}
								 << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("hours.secs") << QVariantHash{}
								<< TestQ {}
								<< static_cast<QObject*>(this)
								<< qMetaTypeId<hours>()
								<< QVariant{}
								<< QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoSeconds), 20}
								<< QJsonValue{QJsonValue::Undefined};
	QTest::newRow("hours.mins") << QVariantHash{}
								<< TestQ {}
								<< static_cast<QObject*>(this)
								<< qMetaTypeId<hours>()
								<< QVariant{}
								<< QCborValue{static_cast<QCborTag>(QCborSerializer::ChronoMinutes), 20}
								<< QJsonValue{QJsonValue::Undefined};
	QTest::newRow("hours.invalid") << QVariantHash{}
								   << TestQ {}
								   << static_cast<QObject*>(this)
								   << qMetaTypeId<hours>()
								   << QVariant{}
								   << QCborValue{QCborKnownTags::Signature, 20}
								   << QJsonValue{QJsonValue::Undefined};
}

QTEST_MAIN(ChronoDurationConverterTest)

#include "tst_chronodurationconverter.moc"

