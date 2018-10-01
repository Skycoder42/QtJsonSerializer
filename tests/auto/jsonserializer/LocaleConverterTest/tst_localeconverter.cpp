#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonlocaleconverter_p.h>

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
	QTest::newRow("locale") << static_cast<int>(QJsonTypeConverter::Standard)
							<< QList<QJsonValue::Type>{QJsonValue::String};
}

void LocaleConverterTest::addMetaData()
{
	QTest::newRow("locale") << static_cast<int>(QMetaType::QLocale)
							<< true;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QString)
							 << false;
}

void LocaleConverterTest::addCommonSerData()
{
	QTest::newRow("normal") << QVariantHash{}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< static_cast<int>(QMetaType::QLocale)
							<< QVariant{QLocale{QLocale::German, QLocale::Germany}}
							<< QJsonValue{QStringLiteral("de_DE")};
	QTest::newRow("c") << QVariantHash{}
					   << TestQ{}
					   << static_cast<QObject*>(nullptr)
					   << static_cast<int>(QMetaType::QLocale)
					   << QVariant{QLocale::c()}
					   << QJsonValue{QStringLiteral("C")};
	QTest::newRow("bcp47.default") << QVariantHash{{QStringLiteral("useBcp47Locale"), true}}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QLocale)
								   << QVariant{QLocale{QLocale::German, QLocale::Germany}}
								   << QJsonValue{QStringLiteral("de")};
	QTest::newRow("bcp47.special") << QVariantHash{{QStringLiteral("useBcp47Locale"), true}}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << static_cast<int>(QMetaType::QLocale)
								   << QVariant{QLocale{QLocale::German, QLocale::Austria}}
								   << QJsonValue{QStringLiteral("de-AT")};
}

void LocaleConverterTest::addDeserData()
{
	QTest::newRow("empty") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << static_cast<int>(QMetaType::QLocale)
						   << QVariant{QLocale::c()}
						   << QJsonValue{QString{}};
	QTest::newRow("invalid") << QVariantHash{}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << static_cast<int>(QMetaType::QLocale)
							 << QVariant{}
							 << QJsonValue{QStringLiteral("some random text")};
}

QTEST_MAIN(LocaleConverterTest)

#include "tst_localeconverter.moc"
