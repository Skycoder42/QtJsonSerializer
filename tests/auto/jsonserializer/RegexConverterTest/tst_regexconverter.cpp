#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonregularexpressionconverter_p.h>

class RegexConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	QJsonRegularExpressionConverter _converter;
};

QJsonTypeConverter *RegexConverterTest::converter()
{
	return &_converter;
}

void RegexConverterTest::addConverterData()
{
	QTest::newRow("regex") << static_cast<int>(QJsonTypeConverter::Standard)
						   << QList<QJsonValue::Type>{QJsonValue::Object, QJsonValue::String};
}

void RegexConverterTest::addMetaData()
{
	QTest::newRow("regex") << static_cast<int>(QMetaType::QRegularExpression)
						   << true;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::QString)
							 << false;
}

void RegexConverterTest::addCommonSerData()
{
	QTest::newRow("simple") << QVariantHash{}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< static_cast<int>(QMetaType::QRegularExpression)
							<< QVariant{QRegularExpression{QStringLiteral("^\\w*m$")}}
							<< QJsonValue{QJsonObject{
									  {QStringLiteral("pattern"), QStringLiteral("^\\w*m$")},
									  {QStringLiteral("options"), static_cast<int>(QRegularExpression::NoPatternOption)}
								  }};
	QTest::newRow("opts") << QVariantHash{}
						  << TestQ{}
						  << static_cast<QObject*>(nullptr)
						  << static_cast<int>(QMetaType::QRegularExpression)
						  << QVariant{QRegularExpression{QStringLiteral("^\\w*m$"), QRegularExpression::CaseInsensitiveOption}}
						  << QJsonValue{QJsonObject{
									{QStringLiteral("pattern"), QStringLiteral("^\\w*m$")},
									{QStringLiteral("options"), static_cast<int>(QRegularExpression::CaseInsensitiveOption)}
								}};
}

void RegexConverterTest::addDeserData()
{
	QTest::newRow("string") << QVariantHash{}
							<< TestQ{}
							<< static_cast<QObject*>(nullptr)
							<< static_cast<int>(QMetaType::QRegularExpression)
							<< QVariant{QRegularExpression{QStringLiteral("just\\sa\\sstring")}}
							<< QJsonValue{QStringLiteral("just\\sa\\sstring")};
}

QTEST_MAIN(RegexConverterTest)

#include "tst_regexconverter.moc"
