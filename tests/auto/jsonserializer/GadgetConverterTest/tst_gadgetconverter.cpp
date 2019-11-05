#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"
#include "testgadget.h"

#include <QtJsonSerializer/private/gadgetconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class GadgetConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;

	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

	bool compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line) override;

private:
	GadgetConverter _converter;
};

void GadgetConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<TestGadget>();
}

TypeConverter *GadgetConverterTest::converter()
{
	return &_converter;
}

void GadgetConverterTest::addConverterData()
{
	QTest::newRow("gadget") << static_cast<int>(TypeConverter::Standard);
}

void GadgetConverterTest::addMetaData()
{

	QTest::newRow("basic") << qMetaTypeId<TestGadget>()
						   << static_cast<QCborTag>(CborSerializer::NoTag)
						   << QCborValue::Map
						   << true
						   << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("basic.null") << qMetaTypeId<TestGadget>()
								<< static_cast<QCborTag>(CborSerializer::NoTag)
								<< QCborValue::Null
								<< true
								<< TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("ptr") << qMetaTypeId<TestGadget*>()
						 << static_cast<QCborTag>(CborSerializer::NoTag)
						 << QCborValue::Map
						 << true
						 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("ptr.null") << qMetaTypeId<TestGadget*>()
							  << static_cast<QCborTag>(CborSerializer::NoTag)
							  << QCborValue::Null
							  << true
							  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("excluded.keysequence") << static_cast<int>(QMetaType::QKeySequence)
										  << static_cast<QCborTag>(CborSerializer::NoTag)
										  << QCborValue::Map
										  << false
										  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("excluded.font") << static_cast<int>(QMetaType::QFont)
								   << static_cast<QCborTag>(CborSerializer::NoTag)
								   << QCborValue::Map
								   << false
								   << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("excluded.locale") << static_cast<int>(QMetaType::QLocale)
									 << static_cast<QCborTag>(CborSerializer::NoTag)
									 << QCborValue::Map
									 << false
									 << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.none") << qMetaTypeId<OpaqueDummy>()
								  << static_cast<QCborTag>(CborSerializer::NoTag)
								  << QCborValue::Map
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.object") << static_cast<int>(QMetaType::QObjectStar)
									<< static_cast<QCborTag>(CborSerializer::NoTag)
									<< QCborValue::Map
									<< false
									<< TypeConverter::DeserializationCapabilityResult::Negative;
}

void GadgetConverterTest::addCommonSerData()
{
	QTest::newRow("basic") << QVariantHash{}
						   << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<TestGadget>()
						   << QVariant::fromValue(TestGadget{10, 0.1, 11})
						   << QCborValue{QCborMap{
									{QStringLiteral("key"), 1},
									{QStringLiteral("value"), 2}
								}}
						   << QJsonValue{QJsonObject{
									{QStringLiteral("key"), 1},
									{QStringLiteral("value"), 2}
								}};
	QTest::newRow("ptr.value") << QVariantHash{}
							   << TestQ{{QMetaType::Int, 5, 11}, {QMetaType::Double, 0.5, 22}}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<TestGadget*>()
							   << QVariant::fromValue(new TestGadget{5, 0.5, 11})
							   << QCborValue{QCborMap{
									  {QStringLiteral("key"), 11},
									  {QStringLiteral("value"), 22}
								  }}
							   << QJsonValue{QJsonObject{
										{QStringLiteral("key"), 11},
										{QStringLiteral("value"), 22}
									}};
	QTest::newRow("ptr.null") << QVariantHash{}
							  << TestQ{}
							  << static_cast<QObject*>(nullptr)
							  << qMetaTypeId<TestGadget*>()
							  << QVariant::fromValue<TestGadget*>(nullptr)
							  << QCborValue{QCborValue::Null}
							  << QJsonValue{QJsonValue::Null};

	QTest::newRow("stored.ignore") << QVariantHash{{QStringLiteral("ignoreStoredAttribute"), true}}
								   << TestQ{
										{QMetaType::Int, 10, 1},
										{QMetaType::Double, 0.1, 2},
										{QMetaType::Int, 42, 3}
								   }
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<TestGadget>()
								   << QVariant::fromValue(TestGadget{10, 0.1, 42})
								   << QCborValue{QCborMap{
										  {QStringLiteral("key"), 1},
										  {QStringLiteral("value"), 2},
										  {QStringLiteral("zhidden"), 3}
									  }}
								   << QJsonValue{QJsonObject{
											{QStringLiteral("key"), 1},
											{QStringLiteral("value"), 2},
											{QStringLiteral("zhidden"), 3}
										}};
}

void GadgetConverterTest::addDeserData()
{
//	QTest::newRow("basic.null") << QVariantHash{}
//								<< TestQ{}
//								<< static_cast<QObject*>(nullptr)
//								<< qMetaTypeId<TestGadget>()
//								<< QVariant{}
//								<< QCborValue{QCborValue::Null}
//								<< QJsonValue{QJsonValue::Null};

	QTest::newRow("validate.none") << QVariantHash{{QStringLiteral("validationFlags"), QVariant::fromValue<JsonSerializer::ValidationFlags>(JsonSerializer::ValidationFlag::StandardValidation)}}
								   << TestQ{{QMetaType::Int, 10, 1}}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<TestGadget>()
								   << QVariant::fromValue(TestGadget{10, 0, 11})
								   << QCborValue{QCborMap{
											{QStringLiteral("key"), 1},
											{QStringLiteral("extra"), 24}
										}}
								   << QJsonValue{QJsonObject{
											{QStringLiteral("key"), 1},
											{QStringLiteral("extra"), 24}
										}};
	QTest::newRow("validate.extra.invalid") << QVariantHash{{QStringLiteral("validationFlags"), QVariant::fromValue<JsonSerializer::ValidationFlags>(JsonSerializer::ValidationFlag::NoExtraProperties)}}
											<< TestQ{{QMetaType::Int, 10, 1}}
											<< static_cast<QObject*>(nullptr)
											<< qMetaTypeId<TestGadget>()
											<< QVariant{}
											<< QCborValue{QCborMap{
													{QStringLiteral("key"), 1},
													{QStringLiteral("extra"), 24}
												}}
											<< QJsonValue{QJsonObject{
													{QStringLiteral("key"), 1},
													{QStringLiteral("extra"), 24}
												}};
	QTest::newRow("validate.extra.valid") << QVariantHash{{QStringLiteral("validationFlags"), QVariant::fromValue<JsonSerializer::ValidationFlags>(JsonSerializer::ValidationFlag::NoExtraProperties)}}
										  << TestQ{{QMetaType::Int, 10, 1}}
										  << static_cast<QObject*>(nullptr)
										  << qMetaTypeId<TestGadget>()
										  << QVariant::fromValue(TestGadget{10, 0, 11})
										  << QCborValue{QCborMap{
													{QStringLiteral("key"), 1}
												}}
										  << QJsonValue{QJsonObject{
													{QStringLiteral("key"), 1}
												}};
	QTest::newRow("validate.all.invalid") << QVariantHash{{QStringLiteral("validationFlags"), QVariant::fromValue<JsonSerializer::ValidationFlags>(JsonSerializer::ValidationFlag::AllProperties)}}
										  << TestQ{{QMetaType::Int, 10, 1}}
										  << static_cast<QObject*>(nullptr)
										  << qMetaTypeId<TestGadget>()
										  << QVariant{}
										  << QCborValue{QCborMap{
													{QStringLiteral("key"), 1},
													{QStringLiteral("extra"), 24}
												}}
										  << QJsonValue{QJsonObject{
													{QStringLiteral("key"), 1},
													{QStringLiteral("extra"), 24}
												}};
	QTest::newRow("validate.all.valid") << QVariantHash{{QStringLiteral("validationFlags"), QVariant::fromValue<JsonSerializer::ValidationFlags>(JsonSerializer::ValidationFlag::AllProperties)}}
										<< TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 10.1, 2}}
										<< static_cast<QObject*>(nullptr)
										<< qMetaTypeId<TestGadget>()
										<< QVariant::fromValue(TestGadget{10, 10.1, 11})
										<< QCborValue{QCborMap{
												{QStringLiteral("key"), 1},
												{QStringLiteral("value"), 2},
												{QStringLiteral("extra"), 24}
											}}
										<< QJsonValue{QJsonObject{
												{QStringLiteral("key"), 1},
												{QStringLiteral("value"), 2},
												{QStringLiteral("extra"), 24}
											}};
	QTest::newRow("validate.full.invalid") << QVariantHash{{QStringLiteral("validationFlags"), QVariant::fromValue<JsonSerializer::ValidationFlags>(JsonSerializer::ValidationFlag::FullPropertyValidation)}}
										   << TestQ{{QMetaType::Int, 10, 1}}
										   << static_cast<QObject*>(nullptr)
										   << qMetaTypeId<TestGadget>()
										   << QVariant{}
										   << QCborValue{QCborMap{
													{QStringLiteral("key"), 1},
													{QStringLiteral("extra"), 24}
												}}
										   << QJsonValue{QJsonObject{
													{QStringLiteral("key"), 1},
													{QStringLiteral("extra"), 24}
												}};
	QTest::newRow("validate.full.valid") << QVariantHash{{QStringLiteral("validationFlags"), QVariant::fromValue<JsonSerializer::ValidationFlags>(JsonSerializer::ValidationFlag::FullPropertyValidation)}}
										 << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 10.1, 2}}
										 << static_cast<QObject*>(nullptr)
										 << qMetaTypeId<TestGadget>()
										 << QVariant::fromValue(TestGadget{10, 10.1, 11})
										 << QCborValue{QCborMap{
												{QStringLiteral("key"), 1},
												{QStringLiteral("value"), 2}
											}}
										 << QJsonValue{QJsonObject{
												{QStringLiteral("key"), 1},
												{QStringLiteral("value"), 2}
											}};
}

bool GadgetConverterTest::compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line)
{
	if (QMetaType::typeFlags(type).testFlag(QMetaType::PointerToGadget)) {
		const auto ptr1 = reinterpret_cast<const TestGadget* const *>(actual.constData());
		const auto ptr2 = reinterpret_cast<const TestGadget* const *>(expected.constData());
		if (ptr1 != ptr2) { // same object is automatically equal
			if (!QTest::qVerify(ptr1, aName, "variant pointer verification", file, line))
				return false;
			if (!QTest::qVerify(ptr2, eName, "variant pointer verification", file, line))
				return false;
			const auto gadPtr1 = *ptr1;
			const auto gadPtr2 = *ptr2;
			if (gadPtr1 != gadPtr2) { //same object is automatically equal
				if (!QTest::qVerify(gadPtr1, aName, "gadget pointer verification", file, line))
					return false;
				if (!QTest::qVerify(gadPtr2, eName, "gadget pointer verification", file, line))
					return false;
				const auto &gad1 = *gadPtr1;
				const auto &gad2 = *gadPtr2;
				return QTest::qCompare(gad1, gad2, aName, eName, file, line);
			} else
				return true;
		} else
			return true;
	} else
		return TypeConverterTestBase::compare(type, actual, expected, aName, eName, file, line);
}

QTEST_MAIN(GadgetConverterTest)

#include "tst_gadgetconverter.moc"
