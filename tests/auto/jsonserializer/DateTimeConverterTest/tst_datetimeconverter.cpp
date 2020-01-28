#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/datetimeconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class DateTimeConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;
	void addDeserData() override;

private:
	DateTimeConverter _converter;
};

TypeConverter *DateTimeConverterTest::converter()
{
	return &_converter;
}

void DateTimeConverterTest::addConverterData()
{
	QTest::newRow("datetime") << static_cast<int>(TypeConverter::Standard);
}

void DateTimeConverterTest::addMetaData()
{
	QTest::newRow("dt.string") << static_cast<int>(QMetaType::QDateTime)
							   << static_cast<QCborTag>(QCborKnownTags::DateTimeString)
							   << QCborValue::String
							   << true
							   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("dt.string.guessed") << static_cast<int>(QMetaType::UnknownType)
									   << static_cast<QCborTag>(QCborKnownTags::DateTimeString)
									   << QCborValue::String
									   << false
									   << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("dt.string.invalid") << static_cast<int>(QMetaType::QDateTime)
									   << static_cast<QCborTag>(QCborKnownTags::DateTimeString)
									   << QCborValue::Integer
									   << true
									   << TypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("dt.tstamp") << static_cast<int>(QMetaType::QDateTime)
							   << static_cast<QCborTag>(QCborKnownTags::UnixTime_t)
							   << QCborValue::Integer
							   << true
							   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("dt.tstamp.guessed") << static_cast<int>(QMetaType::UnknownType)
									   << static_cast<QCborTag>(QCborKnownTags::UnixTime_t)
									   << QCborValue::Integer
									   << false
									   << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("dt.tstamp.invalid") << static_cast<int>(QMetaType::QDateTime)
									   << static_cast<QCborTag>(QCborKnownTags::UnixTime_t)
									   << QCborValue::String
									   << true
									   << TypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("date.date") << static_cast<int>(QMetaType::QDate)
							   << static_cast<QCborTag>(CborSerializer::Date)
							   << QCborValue::String
							   << true
							   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("date.date.guessed") << static_cast<int>(QMetaType::UnknownType)
									   << static_cast<QCborTag>(CborSerializer::Date)
									   << QCborValue::String
									   << false
									   << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("date.date.invalid") << static_cast<int>(QMetaType::QDate)
									   << static_cast<QCborTag>(CborSerializer::Date)
									   << QCborValue::Integer
									   << true
									   << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("date.dt") << static_cast<int>(QMetaType::QDate)
							 << static_cast<QCborTag>(QCborKnownTags::DateTimeString)
							 << QCborValue::String
							 << true
							 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("date.dt.invalid") << static_cast<int>(QMetaType::QDate)
									 << static_cast<QCborTag>(QCborKnownTags::DateTimeString)
									 << QCborValue::Integer
									 << true
									 << TypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("time.time") << static_cast<int>(QMetaType::QTime)
							   << static_cast<QCborTag>(CborSerializer::Time)
							   << QCborValue::String
							   << true
							   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("time.time.guessed") << static_cast<int>(QMetaType::UnknownType)
									   << static_cast<QCborTag>(CborSerializer::Time)
									   << QCborValue::String
									   << false
									   << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("time.time.invalid") << static_cast<int>(QMetaType::QTime)
									   << static_cast<QCborTag>(CborSerializer::Time)
									   << QCborValue::Integer
									   << true
									   << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("time.dt") << static_cast<int>(QMetaType::QTime)
							 << static_cast<QCborTag>(QCborKnownTags::DateTimeString)
							 << QCborValue::String
							 << true
							 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("time.dt.invalid") << static_cast<int>(QMetaType::QTime)
									 << static_cast<QCborTag>(QCborKnownTags::DateTimeString)
									 << QCborValue::Integer
									 << true
									 << TypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QString)
								  << static_cast<QCborTag>(QCborKnownTags::DateTimeString)
								  << QCborValue::String
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag.dt") << static_cast<int>(QMetaType::QDateTime)
									<< static_cast<QCborTag>(CborSerializer::Date)
									<< QCborValue::String
									<< true
									<< TypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("invalid.tag.date") << static_cast<int>(QMetaType::QDate)
									  << static_cast<QCborTag>(CborSerializer::Time)
									  << QCborValue::String
									  << true
									  << TypeConverter::DeserializationCapabilityResult::WrongTag;
	QTest::newRow("invalid.tag.time") << static_cast<int>(QMetaType::QTime)
									  << static_cast<QCborTag>(CborSerializer::Date)
									  << QCborValue::String
									  << true
									  << TypeConverter::DeserializationCapabilityResult::WrongTag;

}

void DateTimeConverterTest::addCommonSerData()
{
	QTest::newRow("datetime.string") << QVariantHash{}
									 << TestQ{}
									 << static_cast<QObject*>(nullptr)
									 << static_cast<int>(QMetaType::QDateTime)
									 << QVariant{QDateTime{{2019, 1, 3}, {13, 42, 15, 563}}}
									 << QCborValue{QDateTime{{2019, 1, 3}, {13, 42, 15, 563}}}
									 << QJsonValue{QStringLiteral("2019-01-03T13:42:15.563")};
	QTest::newRow("datetime.string.empty") << QVariantHash{}
										   << TestQ{}
										   << static_cast<QObject*>(nullptr)
										   << static_cast<int>(QMetaType::QDateTime)
										   << QVariant{QDateTime{}}
										   << QCborValue{QDateTime{}}
										   << QJsonValue{QString{}};
	QTest::newRow("datetime.string.tag") << QVariantHash{}
										 << TestQ{}
										 << static_cast<QObject*>(nullptr)
										 << static_cast<int>(QMetaType::QDateTime)
										 << QVariant{QDateTime{{2019, 1, 3}, {13, 42, 15, 563}}}
										 << QCborValue{QCborKnownTags::DateTimeString, QStringLiteral("2019-01-03T13:42:15.563")}
										 << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("datetime.tstamp") << QVariantHash{{QStringLiteral("dateAsTimeStamp"), true}}
									 << TestQ{}
									 << static_cast<QObject*>(nullptr)
									 << static_cast<int>(QMetaType::QDateTime)
									 << QVariant{QDateTime{{2019, 1, 3}, {13, 42, 15}, Qt::UTC}}
									 << QCborValue{QCborKnownTags::UnixTime_t, 1546522935ll}
									 << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("date.datestr") << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QDate)
								  << QVariant{QDate{2019, 1, 3}}
								  << QCborValue{static_cast<QCborTag>(CborSerializer::Date), QStringLiteral("2019-01-03")}
								  << QJsonValue{QStringLiteral("2019-01-03")};
	QTest::newRow("date.datestr.empty") << QVariantHash{}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QDate)
										<< QVariant{QDate{}}
										<< QCborValue{static_cast<QCborTag>(CborSerializer::Date), QString{}}
										<< QJsonValue{QString{}};

	QTest::newRow("time.timestr") << QVariantHash{}
								  << TestQ{}
								  << static_cast<QObject*>(nullptr)
								  << static_cast<int>(QMetaType::QTime)
								  << QVariant{QTime{13, 42, 15, 563}}
								  << QCborValue{static_cast<QCborTag>(CborSerializer::Time), QStringLiteral("13:42:15.563")}
								  << QJsonValue{QStringLiteral("13:42:15.563")};
	QTest::newRow("time.timestr.empty") << QVariantHash{}
										<< TestQ{}
										<< static_cast<QObject*>(nullptr)
										<< static_cast<int>(QMetaType::QTime)
										<< QVariant{QTime{}}
										<< QCborValue{static_cast<QCborTag>(CborSerializer::Time), QString{}}
										<< QJsonValue{QString{}};
}

void DateTimeConverterTest::addSerData()
{
	QTest::newRow("datetime.tstamp.empty") << QVariantHash{{QStringLiteral("dateAsTimeStamp"), true}}
										   << TestQ{}
										   << static_cast<QObject*>(nullptr)
										   << static_cast<int>(QMetaType::QDateTime)
										   << QVariant{QDateTime{}}
										   << QCborValue{QCborKnownTags::UnixTime_t, 0ll}
										   << QJsonValue{QJsonValue::Undefined};
}

void DateTimeConverterTest::addDeserData()
{
	QTest::newRow("datetime.tstamp.empty") << QVariantHash{{QStringLiteral("dateAsTimeStamp"), true}}
										   << TestQ{}
										   << static_cast<QObject*>(nullptr)
										   << static_cast<int>(QMetaType::QDateTime)
										   << QVariant{QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).toLocalTime()}
										   << QCborValue{QCborKnownTags::UnixTime_t, 0ll}
										   << QJsonValue{QJsonValue::Undefined};

	QTest::newRow("date.dtstr") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QDate)
								<< QVariant{QDate{2019, 1, 3}}
								<< QCborValue{QCborKnownTags::DateTimeString, QStringLiteral("2019-01-03T00:00:00.000")}
								<< QJsonValue{QJsonValue::Undefined};

	QTest::newRow("time.dtstr") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QTime)
								<< QVariant{QTime{13, 42, 15, 563}}
								<< QCborValue{QCborKnownTags::DateTimeString, QStringLiteral("1970-01-01T13:42:15.563")}
								<< QJsonValue{QJsonValue::Undefined};
}

QTEST_MAIN(DateTimeConverterTest)

#include "tst_datetimeconverter.moc"
