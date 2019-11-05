#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonlocaleconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class LocaleConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	QJsonLocaleConverter _converter;
};

QJsonTypeConverter *LocaleConverterTest::converter()
{
	return &_converter;
}

void LocaleConverterTest::addConverterData()
{
	QTest::newRow("locale") << static_cast<int>(QJsonTypeConverter::Standard);
}

void LocaleConverterTest::addMetaData()
{
	QTest::newRow("locale.iso") << static_cast<int>(QMetaType::QLocale)
								<< static_cast<QCborTag>(QCborSerializer::LocaleISO)
								<< QCborValue::String
								<< true
								<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("locale.bcp47") << static_cast<int>(QMetaType::QLocale)
								  << static_cast<QCborTag>(QCborSerializer::LocaleBCP47)
								  << QCborValue::String
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("guessed.iso") << static_cast<int>(QMetaType::UnknownType)
								 << static_cast<QCborTag>(QCborSerializer::LocaleISO)
								 << QCborValue::String
								 << false
								 << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("guessed.bcp47") << static_cast<int>(QMetaType::UnknownType)
								   << static_cast<QCborTag>(QCborSerializer::LocaleBCP47)
								   << QCborValue::String
								   << false
								   << QJsonTypeConverter::DeserializationCapabilityResult::Guessed;
	QTest::newRow("invalid.type") << static_cast<int>(QMetaType::QString)
								  << static_cast<QCborTag>(QCborSerializer::LocaleISO)
								  << QCborValue::String
								  << false
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.data") << static_cast<int>(QMetaType::QLocale)
								  << static_cast<QCborTag>(QCborSerializer::LocaleISO)
								  << QCborValue::ByteArray
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << static_cast<int>(QMetaType::QLocale)
								 << static_cast<QCborTag>(QCborSerializer::Font)
								 << QCborValue::String
								 << true
								 << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
}

void LocaleConverterTest::addCommonSerData()
{
	QTest::newRow("normal") << QVariantHash{}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< static_cast<int>(QMetaType::QLocale)
							<< QVariant{QLocale{QLocale::German, QLocale::Germany}}
							<< QCborValue{static_cast<QCborTag>(QCborSerializer::LocaleISO), QStringLiteral("de_DE")}
							<< QJsonValue{QStringLiteral("de_DE")};
	QTest::newRow("c") << QVariantHash{}
					   << TestQ{}
					   << static_cast<QObject*>(nullptr)
					   << static_cast<int>(QMetaType::QLocale)
					   << QVariant{QLocale::c()}
					   << QCborValue{static_cast<QCborTag>(QCborSerializer::LocaleISO), QStringLiteral("C")}
					   << QJsonValue{QStringLiteral("C")};
	QTest::newRow("bcp47.default") << QVariantHash{{QStringLiteral("useBcp47Locale"), true}}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QLocale)
								   << QVariant{QLocale{QLocale::German, QLocale::Germany}}
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::LocaleBCP47), QStringLiteral("de")}
								   << QJsonValue{QStringLiteral("de")};
	QTest::newRow("bcp47.special") << QVariantHash{{QStringLiteral("useBcp47Locale"), true}}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QLocale)
								   << QVariant{QLocale{QLocale::German, QLocale::Austria}}
								   << QCborValue{static_cast<QCborTag>(QCborSerializer::LocaleBCP47), QStringLiteral("de-AT")}
								   << QJsonValue{QStringLiteral("de-AT")};
}

void LocaleConverterTest::addDeserData()
{
	QTest::newRow("empty") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << static_cast<int>(QMetaType::QLocale)
						   << QVariant{QLocale::c()}
						   << QCborValue{static_cast<QCborTag>(QCborSerializer::LocaleISO), QString{}}
						   << QJsonValue{QString{}};
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << static_cast<int>(QMetaType::QLocale)
							 << QVariant{}
							 << QCborValue{static_cast<QCborTag>(QCborSerializer::LocaleISO), QStringLiteral("some random text")}
							 << QJsonValue{QStringLiteral("some random text")};
}

QTEST_MAIN(LocaleConverterTest)

#include "tst_localeconverter.moc"
