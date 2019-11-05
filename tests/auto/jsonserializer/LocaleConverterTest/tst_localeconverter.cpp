#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/localeconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class LocaleConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	LocaleConverter _converter;
};

TypeConverter *LocaleConverterTest::converter()
{
	return &_converter;
}

void LocaleConverterTest::addConverterData()
{
	QTest::newRow("locale") << static_cast<int>(TypeConverter::Standard);
}

void LocaleConverterTest::addMetaData()
{
	QTest::newRow("locale.iso") << static_cast<int>(QMetaType::QLocale)
								<< static_cast<QCborTag>(CborSerializer::LocaleISO)
								<< QCborValue::String
								<< true
								<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("locale.bcp47") << static_cast<int>(QMetaType::QLocale)
								  << static_cast<QCborTag>(CborSerializer::LocaleBCP47)
								  << QCborValue::String
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("guessed.iso") << static_cast<int>(QMetaType::UnknownType)
								 << static_cast<QCborTag>(CborSerializer::LocaleISO)
								 << QCborValue::String
								 << false
								 << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("guessed.bcp47") << static_cast<int>(QMetaType::UnknownType)
								   << static_cast<QCborTag>(CborSerializer::LocaleBCP47)
								   << QCborValue::String
								   << false
								   << TypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QString)
								  << static_cast<QCborTag>(CborSerializer::LocaleISO)
								  << QCborValue::String
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.data") << static_cast<int>(QMetaType::QLocale)
								  << static_cast<QCborTag>(CborSerializer::LocaleISO)
								  << QCborValue::ByteArray
								  << true
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << static_cast<int>(QMetaType::QLocale)
								 << static_cast<QCborTag>(CborSerializer::Font)
								 << QCborValue::String
								 << true
								 << TypeConverter::DeserializationCapabilityResult::WrongTag;
}

void LocaleConverterTest::addCommonSerData()
{
	QTest::newRow("normal") << QVariantHash{}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< static_cast<int>(QMetaType::QLocale)
							<< QVariant{QLocale{QLocale::German, QLocale::Germany}}
							<< QCborValue{static_cast<QCborTag>(CborSerializer::LocaleISO), QStringLiteral("de_DE")}
							<< QJsonValue{QStringLiteral("de_DE")};
	QTest::newRow("c") << QVariantHash{}
					   << TestQ{}
					   << static_cast<QObject*>(nullptr)
					   << static_cast<int>(QMetaType::QLocale)
					   << QVariant{QLocale::c()}
					   << QCborValue{static_cast<QCborTag>(CborSerializer::LocaleISO), QStringLiteral("C")}
					   << QJsonValue{QStringLiteral("C")};
	QTest::newRow("bcp47.default") << QVariantHash{{QStringLiteral("useBcp47Locale"), true}}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QLocale)
								   << QVariant{QLocale{QLocale::German, QLocale::Germany}}
								   << QCborValue{static_cast<QCborTag>(CborSerializer::LocaleBCP47), QStringLiteral("de")}
								   << QJsonValue{QStringLiteral("de")};
	QTest::newRow("bcp47.special") << QVariantHash{{QStringLiteral("useBcp47Locale"), true}}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QLocale)
								   << QVariant{QLocale{QLocale::German, QLocale::Austria}}
								   << QCborValue{static_cast<QCborTag>(CborSerializer::LocaleBCP47), QStringLiteral("de-AT")}
								   << QJsonValue{QStringLiteral("de-AT")};
}

void LocaleConverterTest::addDeserData()
{
	QTest::newRow("empty") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << static_cast<int>(QMetaType::QLocale)
						   << QVariant{QLocale::c()}
						   << QCborValue{static_cast<QCborTag>(CborSerializer::LocaleISO), QString{}}
						   << QJsonValue{QString{}};
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << static_cast<int>(QMetaType::QLocale)
							 << QVariant{}
							 << QCborValue{static_cast<QCborTag>(CborSerializer::LocaleISO), QStringLiteral("some random text")}
							 << QJsonValue{QStringLiteral("some random text")};
}

QTEST_MAIN(LocaleConverterTest)

#include "tst_localeconverter.moc"
