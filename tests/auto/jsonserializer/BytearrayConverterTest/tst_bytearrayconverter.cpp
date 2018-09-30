#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonbytearrayconverter_p.h>

class BytearrayConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	QJsonBytearrayConverter _converter;
};

QJsonTypeConverter *BytearrayConverterTest::converter()
{
	return &_converter;
}

void BytearrayConverterTest::addConverterData()
{
	QTest::newRow("bytearray") << static_cast<int>(QJsonTypeConverter::Standard)
							   << QList<QJsonValue::Type>{QJsonValue::String};
}

void BytearrayConverterTest::addMetaData()
{
	QTest::newRow("bytearray") << static_cast<int>(QMetaType::QByteArray)
							   << true;
	QTest::newRow("invalid.string") << static_cast<int>(QMetaType::QString)
									<< false;
	QTest::newRow("invalid.char") << static_cast<int>(QMetaType::Char)
								  << false;
	QTest::newRow("invalid.string") << static_cast<int>(QMetaType::Int)
									<< false;
}

void BytearrayConverterTest::addCommonSerData()
{
	QTest::newRow("basic") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << static_cast<int>(QMetaType::QByteArray)
						   << QVariant{QByteArrayLiteral("Hello World")}
						   << QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ=")};
}

void BytearrayConverterTest::addDeserData()
{

	QTest::newRow("invalid") << QVariantHash{{QStringLiteral("validateBase64"), false}}
							 << TestQ{}
							 << static_cast<QObject*>(nullptr)
							 << static_cast<int>(QMetaType::QByteArray)
							 << QVariant{QByteArrayLiteral("Hello World8")}
							 << QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ=42")};
	QTest::newRow("validated1") << QVariantHash{{QStringLiteral("validateBase64"), true}}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QByteArray)
								<< QVariant{}
								<< QJsonValue{QStringLiteral("SGVsbG8#'gV29ybGQ=42")};
	QTest::newRow("validated2") << QVariantHash{{QStringLiteral("validateBase64"), true}}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QByteArray)
								<< QVariant{}
								<< QJsonValue{QStringLiteral("SGVsbG8gV29ybGQ2=")};
	QTest::newRow("validated3") << QVariantHash{{QStringLiteral("validateBase64"), true}}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< static_cast<int>(QMetaType::QByteArray)
								<< QVariant{}
								<< QJsonValue{QStringLiteral("SGVsbG%gV29ybGQ=")};
}

QTEST_MAIN(BytearrayConverterTest)

#include "tst_bytearrayconverter.moc"
