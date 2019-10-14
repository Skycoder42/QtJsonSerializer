#include <QtTest>
#include <QtJsonSerializer>

#include "testgadget.h"
#include "testobject.h"

using TestTuple = std::tuple<int, QString, QList<int>>;
using TestPair = std::pair<bool, int>;
using TestVariant = std::variant<bool, int, double>;
Q_DECLARE_METATYPE(TestTuple)
Q_DECLARE_METATYPE(TestPair)
Q_DECLARE_METATYPE(std::optional<int>)
Q_DECLARE_METATYPE(TestVariant)

class CborSerializerTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testSerialization_data();
	void testSerialization();
	void testDeserialization_data();
	void testDeserialization();

	void testDeviceSerialization();

private:
	QCborSerializer *serializer = nullptr;

	void addCommonData();
	void resetProps();
};

void CborSerializerTest::initTestCase()
{
	serializer = new QCborSerializer{this};
}

void CborSerializerTest::cleanupTestCase()
{
	delete serializer;
	serializer = nullptr;
}

void CborSerializerTest::testSerialization_data()
{
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QCborValue>("result");
	QTest::addColumn<bool>("works");
	QTest::addColumn<QVariantHash>("extraProps");
	QTest::addColumn<QCborTag>("typeTag");

	addCommonData();
}

void CborSerializerTest::testSerialization()
{
	QFETCH(QVariant, data);
	QFETCH(QCborValue, result);
	QFETCH(bool, works);
	QFETCH(QVariantHash, extraProps);
	QFETCH(QCborTag, typeTag);

	resetProps();
	for(auto it = extraProps.constBegin(); it != extraProps.constEnd(); it++)
		serializer->setProperty(qUtf8Printable(it.key()), it.value());
	serializer->setTypeTag(data.userType(), typeTag);

	try {
		if(works) {
			auto res = serializer->serialize(data);
			QCOMPARE(res, result);
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->serialize(data), QJsonSerializationException);
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void CborSerializerTest::testDeserialization_data()
{
	QTest::addColumn<QVariant>("result");
	QTest::addColumn<QCborValue>("data");
	QTest::addColumn<bool>("works");
	QTest::addColumn<QVariantHash>("extraProps");
	QTest::addColumn<QCborTag>("typeTag");

	addCommonData();

	QTest::newRow("null.invalid.bool") << QVariant{false}
									   << QCborValue{nullptr}
									   << false
									   << QVariantHash{}
									   << QCborTypeConverter::NoTag;
	QTest::newRow("null.invalid.int") << QVariant{0}
									  << QCborValue{nullptr}
									  << false
									  << QVariantHash{}
									  << QCborTypeConverter::NoTag;
	QTest::newRow("null.invalid.double") << QVariant{0.0}
										 << QCborValue{nullptr}
										 << false
										 << QVariantHash{}
										 << QCborTypeConverter::NoTag;
	QTest::newRow("null.invalid.string") << QVariant{QString()}
										 << QCborValue{nullptr}
										 << false
										 << QVariantHash{}
										 << QCborTypeConverter::NoTag;
	QTest::newRow("null.invalid.bytearray") << QVariant{QByteArray()}
											<< QCborValue{nullptr}
											<< false
											<< QVariantHash{}
											<< QCborTypeConverter::NoTag;
	QTest::newRow("null.invalid.datetime") << QVariant{QDateTime()}
										   << QCborValue{nullptr}
										   << false
										   << QVariantHash{}
										   << QCborTypeConverter::NoTag;
	QTest::newRow("null.invalid.uuid") << QVariant{QUuid()}
									   << QCborValue{nullptr}
									   << false
									   << QVariantHash{}
									   << QCborTypeConverter::NoTag;
	QTest::newRow("null.invalid.url") << QVariant{QUrl()}
									  << QCborValue{nullptr}
									  << false
									  << QVariantHash{}
									  << QCborTypeConverter::NoTag;
	QTest::newRow("null.invalid.regex") << QVariant{QRegularExpression()}
										<< QCborValue{nullptr}
										<< false
										<< QVariantHash{}
										<< QCborTypeConverter::NoTag;

	QTest::newRow("null.valid.bool") << QVariant{false}
									 << QCborValue{nullptr}
									 << true
									 << QVariantHash {
											{QStringLiteral("allowDefaultNull"), true},
											{QStringLiteral("validationFlags"), QVariant::fromValue<QJsonSerializer::ValidationFlags>(QJsonSerializer::StandardValidation)}
										}
									 << QCborTypeConverter::NoTag;
	QTest::newRow("null.valid.int") << QVariant{0}
									<< QCborValue{nullptr}
									<< true
									<< QVariantHash {
										   {QStringLiteral("allowDefaultNull"), true},
										   {QStringLiteral("validationFlags"), QVariant::fromValue<QJsonSerializer::ValidationFlags>(QJsonSerializer::StandardValidation)}
									   }
									<< QCborTypeConverter::NoTag;
	QTest::newRow("null.valid.double") << QVariant{0.0}
									   << QCborValue{nullptr}
									   << false
									   << QVariantHash{}
									   << QCborTypeConverter::NoTag;
	QTest::newRow("null.valid.string") << QVariant{QString()}
									   << QCborValue{nullptr}
									   << true
									   << QVariantHash {
											  {QStringLiteral("allowDefaultNull"), true},
											  {QStringLiteral("validationFlags"), QVariant::fromValue<QJsonSerializer::ValidationFlags>(QJsonSerializer::StandardValidation)}
										  }
									   << QCborTypeConverter::NoTag;
	QTest::newRow("null.valid.bytearray") << QVariant{QByteArray()}
										  << QCborValue{nullptr}
										  << true
										  << QVariantHash {
												 {QStringLiteral("allowDefaultNull"), true},
												 {QStringLiteral("validationFlags"), QVariant::fromValue<QJsonSerializer::ValidationFlags>(QJsonSerializer::StandardValidation)}
											 }
										  << QCborTypeConverter::NoTag;
	QTest::newRow("null.valid.datetime") << QVariant{QDateTime()}
										 << QCborValue{nullptr}
										 << true
										 << QVariantHash {
												{QStringLiteral("allowDefaultNull"), true},
												{QStringLiteral("validationFlags"), QVariant::fromValue<QJsonSerializer::ValidationFlags>(QJsonSerializer::StandardValidation)}
											}
										 << QCborTypeConverter::NoTag;
	QTest::newRow("null.valid.uuid") << QVariant{QUuid()}
									 << QCborValue{nullptr}
									 << true
									 << QVariantHash {
											{QStringLiteral("allowDefaultNull"), true},
											{QStringLiteral("validationFlags"), QVariant::fromValue<QJsonSerializer::ValidationFlags>(QJsonSerializer::StandardValidation)}
										}
									 << QCborTypeConverter::NoTag;
	QTest::newRow("null.valid.url") << QVariant{QUrl()}
									<< QCborValue{nullptr}
									<< true
									<< QVariantHash {
										   {QStringLiteral("allowDefaultNull"), true},
										   {QStringLiteral("validationFlags"), QVariant::fromValue<QJsonSerializer::ValidationFlags>(QJsonSerializer::StandardValidation)}
									   }
									<< QCborTypeConverter::NoTag;
	QTest::newRow("null.valid.regex") << QVariant{QRegularExpression()}
									  << QCborValue{nullptr}
									  << true
									  << QVariantHash {
											 {QStringLiteral("allowDefaultNull"), true},
											 {QStringLiteral("validationFlags"), QVariant::fromValue<QJsonSerializer::ValidationFlags>(QJsonSerializer::StandardValidation)}
										 }
									  << QCborTypeConverter::NoTag;

	QTest::newRow("strict.invalid.bool") << QVariant{true}
										 << QCborValue{1}
										 << false
										 << QVariantHash{}
										 << QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.int") << QVariant{4}
										<< QCborValue{4.2}
										<< false
										<< QVariantHash{}
										<< QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.double") << QVariant{4.2}
										   << QCborValue{4}
										   << false
										   << QVariantHash{}
										   << QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.string") << QVariant{QString{}}
										   << QCborValue{QByteArray{"test"}}
										   << false
										   << QVariantHash{}
										   << QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.bytearray") << QVariant{QByteArray{}}
											  << QCborValue{QStringLiteral("test")}
											  << false
											  << QVariantHash{}
											  << QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.int") << QVariant::fromValue(nullptr)
										<< QCborValue{0}
										<< false
										<< QVariantHash{}
										<< QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.datetime") << QVariant{QDateTime{}}
											 << QCborValue{QDateTime::currentDateTime().toString(Qt::ISODateWithMs)}
											 << false
											 << QVariantHash{}
											 << QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.url") << QVariant{QUrl{}}
										<< QCborValue{QStringLiteral("https://test.de")}
										<< false
										<< QVariantHash{}
										<< QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.uuid") << QVariant{QUuid{}}
										 << QCborValue{QUuid::createUuid().toRfc4122()}
										 << false
										 << QVariantHash{}
										 << QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.regex") << QVariant{QRegularExpression{}}
										  << QCborValue{QStringLiteral("test")}
										  << false
										  << QVariantHash{}
										  << QCborTypeConverter::NoTag;
	QTest::newRow("strict.invalid.tagged") << QVariant{QString{}}
										   << QCborValue{QStringLiteral("test")}
										   << false
										   << QVariantHash{}
										   << static_cast<QCborTag>(QCborKnownTags::Base64);
}

void CborSerializerTest::testDeserialization()
{
	QFETCH(QCborValue, data);
	QFETCH(QVariant, result);
	QFETCH(bool, works);
	QFETCH(QVariantHash, extraProps);
	QFETCH(QCborTag, typeTag);

	resetProps();
	for(auto it = extraProps.constBegin(); it != extraProps.constEnd(); it++)
		serializer->setProperty(qUtf8Printable(it.key()), it.value());
	serializer->setTypeTag(result.userType(), typeTag);

	try {
		if(works) {
			auto res = serializer->deserialize(data, result.userType(), this);
			if(result.userType() == qMetaTypeId<TestObject*>())
				QVERIFY(TestObject::equals(res.value<TestObject*>(), result.value<TestObject*>()));
			else
				QCOMPARE(res, result);
		} else
			QVERIFY_EXCEPTION_THROWN(serializer->deserialize(data, result.userType(), this), QJsonDeserializationException);
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void CborSerializerTest::testDeviceSerialization()
{

}

void CborSerializerTest::addCommonData()
{
	QTest::newRow("bool.true") << QVariant{true}
							   << QCborValue{true}
							   << true
							   << QVariantHash{}
							   << QCborTypeConverter::NoTag;
	QTest::newRow("bool.false") << QVariant{false}
								<< QCborValue{false}
								<< true
								<< QVariantHash{}
								<< QCborTypeConverter::NoTag;
	QTest::newRow("int") << QVariant{42}
						 << QCborValue{42}
						 << true
						 << QVariantHash{}
						 << QCborTypeConverter::NoTag;
	QTest::newRow("double") << QVariant{4.2}
							<< QCborValue{4.2}
							<< true
							<< QVariantHash{}
							<< QCborTypeConverter::NoTag;
	QTest::newRow("string.normal") << QVariant{QStringLiteral("baum")}
								   << QCborValue{QStringLiteral("baum")}
								   << true
								   << QVariantHash{}
								   << QCborTypeConverter::NoTag;
	QTest::newRow("string.empty") << QVariant{QString()}
								  << QCborValue{QString()}
								  << true
								  << QVariantHash{}
								  << QCborTypeConverter::NoTag;
	QTest::newRow("string.base64") << QVariant{QStringLiteral("AA==")}
								   << QCborValue{QCborKnownTags::Base64, QStringLiteral("AA==")}
								   << true
								   << QVariantHash{}
								   << static_cast<QCborTag>(QCborKnownTags::Base64);
	QTest::newRow("string.base64url") << QVariant{QStringLiteral("AA")}
									  << QCborValue{QCborKnownTags::Base64url, QStringLiteral("AA")}
									  << true
									  << QVariantHash{}
									  << static_cast<QCborTag>(QCborKnownTags::Base64url);
	QTest::newRow("bytearray.normal") << QVariant{QByteArray("baum")}
									  << QCborValue{QByteArray("baum")}
									  << true
									  << QVariantHash{}
									  << QCborTypeConverter::NoTag;
	QTest::newRow("bytearray.empty") << QVariant{QByteArray()}
									 << QCborValue{QByteArray()}
									 << true
									 << QVariantHash{}
									 << QCborTypeConverter::NoTag;
	QTest::newRow("bytearray.base64") << QVariant{QByteArray("baum")}
									  << QCborValue{QCborKnownTags::ExpectedBase64, QByteArray("baum")}
									  << true
									  << QVariantHash{}
									  << static_cast<QCborTag>(QCborKnownTags::ExpectedBase64);
	QTest::newRow("bytearray.base64url") << QVariant{QByteArray("baum")}
										 << QCborValue{QCborKnownTags::ExpectedBase64url, QByteArray("baum")}
										 << true
										 << QVariantHash{}
										 << static_cast<QCborTag>(QCborKnownTags::ExpectedBase64url);
	QTest::newRow("bytearray.base16") << QVariant{QByteArray("baum")}
									  << QCborValue{QCborKnownTags::ExpectedBase16, QByteArray("baum")}
									  << true
									  << QVariantHash{}
									  << static_cast<QCborTag>(QCborKnownTags::ExpectedBase16);
	QTest::newRow("nullptr") << QVariant::fromValue(nullptr)
							 << QCborValue{nullptr}
							 << true
							 << QVariantHash{}
							 << QCborTypeConverter::NoTag;

	QTest::newRow("date.valid") << QVariant{QDate{2010, 10, 20}}
								<< QCborValue{QDateTime{QDate{2010, 10, 20}}}
								<< true
								<< QVariantHash{}
								<< QCborTypeConverter::NoTag;
	QTest::newRow("date.invalid") << QVariant{QDate{}}
								  << QCborValue{QDateTime{}}
								  << true
								  << QVariantHash{}
								  << QCborTypeConverter::NoTag;
	QTest::newRow("time.valid") << QVariant{QTime{14, 30, 15, 123}}
								<< QCborValue{QDateTime{QDate{1970, 1, 1}, QTime{14, 30, 15, 123}}}
								<< true
								<< QVariantHash{}
								<< QCborTypeConverter::NoTag;
	QTest::newRow("time.invalid") << QVariant{QTime{}}
								  << QCborValue{QDateTime{}}
								  << true
								  << QVariantHash{}
								  << QCborTypeConverter::NoTag;
	QTest::newRow("datetime.valid") << QVariant{QDateTime{QDate{2010, 10, 20}, QTime{14, 30}}}
									<< QCborValue{QDateTime{QDate{2010, 10, 20}, QTime{14, 30}}}
									<< true
									<< QVariantHash{}
									<< QCborTypeConverter::NoTag;
	QTest::newRow("datetime.invalid") << QVariant{QDateTime{}}
									  << QCborValue{QDateTime{}}
									  << true
									  << QVariantHash{}
									  << QCborTypeConverter::NoTag;
	QTest::newRow("datetime.tagged") << QVariant{QDateTime{QDate{2010, 10, 20}, QTime{14, 30}}}
									 << QCborValue{QCborKnownTags::DateTimeString, QStringLiteral("2010-10-20T14:30:00.000")}
									 << true
									 << QVariantHash{}
									 << QCborTypeConverter::NoTag;
	auto dt = QDateTime::fromSecsSinceEpoch(QDateTime::currentDateTime().toSecsSinceEpoch());
	QTest::newRow("datetime.unix") << QVariant{dt}
								   << QCborValue{QCborKnownTags::UnixTime_t, dt.toUTC().toSecsSinceEpoch()}
								   << true
								   << QVariantHash{}
								   << static_cast<QCborTag>(QCborKnownTags::UnixTime_t);
	auto id = QUuid::createUuid();
	QTest::newRow("uuid.normal") << QVariant{id}
								 << QCborValue{id}
								 << true
								 << QVariantHash{}
								 << QCborTypeConverter::NoTag;
	QTest::newRow("uuid.tagged") << QVariant{id}
								 << QCborValue{QCborKnownTags::Uuid, id.toRfc4122()}
								 << true
								 << QVariantHash{}
								 << QCborTypeConverter::NoTag;
	QTest::newRow("url.valid") << QVariant{QUrl{QStringLiteral("https://example.com/test.xml?baum=42#tree")}}
							   << QCborValue{QUrl{QStringLiteral("https://example.com/test.xml?baum=42#tree")}}
							   << true
							   << QVariantHash{}
							   << QCborTypeConverter::NoTag;
	QTest::newRow("url.invalid") << QVariant{QUrl{}}
								 << QCborValue{QUrl{}}
								 << true
								 << QVariantHash{}
								 << QCborTypeConverter::NoTag;
	QTest::newRow("url.tagged") << QVariant{QUrl{QStringLiteral("https://example.com/test.xml?baum=42#tree")}}
								<< QCborValue{QCborKnownTags::Url, QStringLiteral("https://example.com/test.xml?baum=42#tree")}
								<< true
								<< QVariantHash{}
								<< QCborTypeConverter::NoTag;
	QTest::newRow("regex.valid") << QVariant{QRegularExpression{QStringLiteral(R"__(^[Hh]ello\s+world!?$)__")}}
								 << QCborValue{QRegularExpression{QStringLiteral(R"__(^[Hh]ello\s+world!?$)__")}}
								 << true
								 << QVariantHash{}
								 << QCborTypeConverter::NoTag;
	QTest::newRow("regex.invalid") << QVariant{QRegularExpression{}}
								   << QCborValue{QRegularExpression{}}
								   << true
								   << QVariantHash{}
								   << QCborTypeConverter::NoTag;
	QTest::newRow("regex.tagged") << QVariant{QRegularExpression{QStringLiteral(R"__(^[Hh]ello\s+world!?$)__")}}
								  << QCborValue{QCborKnownTags::RegularExpression, QStringLiteral(R"__(^[Hh]ello\s+world!?$)__")}
								  << true
								  << QVariantHash{}
								  << QCborTypeConverter::NoTag;
}

void CborSerializerTest::resetProps()
{
	serializer->setAllowDefaultNull(false);
	serializer->setKeepObjectName(false);
	serializer->setEnumAsString(false);
	serializer->setValidateBase64(true);
	serializer->setUseBcp47Locale(true);
	serializer->setValidationFlags(QJsonSerializer::StrictBasicTypes);
	serializer->setPolymorphing(QJsonSerializer::Enabled);
}

QTEST_MAIN(CborSerializerTest)

#include "tst_cborserializer.moc"

