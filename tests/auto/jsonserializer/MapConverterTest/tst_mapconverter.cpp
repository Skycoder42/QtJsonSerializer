#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonmapconverter_p.h>

class MapConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;

private:
	QJsonMapConverter _converter;
};

void MapConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<QMap<QString, int>>();
	QMetaType::registerEqualsComparator<QHash<QString, int>>();
}

QJsonTypeConverter *MapConverterTest::converter()
{
	return &_converter;
}

void MapConverterTest::addConverterData()
{
	QTest::newRow("map") << static_cast<int>(QJsonTypeConverter::Standard);
}

void MapConverterTest::addMetaData()
{
	QTest::newRow("int") << qMetaTypeId<QMap<QString, int>>()
						 << QJsonTypeConverter::NoTag
						 << QCborValue::Map
						 << true
						 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("string") << qMetaTypeId<QMap<QString, QString>>()
							<< static_cast<QCborTag>(QCborSerializer::ExplicitMap)
							<< QCborValue::Map
							<< true
							<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("variant") << static_cast<int>(QMetaType::QVariantMap)
							 << QJsonTypeConverter::NoTag
							 << QCborValue::Map
							 << true
							 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("map") << qMetaTypeId<QMap<QString, QList<int>>>()
						 << QJsonTypeConverter::NoTag
						 << QCborValue::Map
						 << true
						 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("pair") << qMetaTypeId<QMap<QString, QPair<int, bool>>>()
						  << QJsonTypeConverter::NoTag
						  << QCborValue::Map
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;

	QTest::newRow("hash") << qMetaTypeId<QHash<QString, int>>()
						  << QJsonTypeConverter::NoTag
						  << QCborValue::Map
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;

	QTest::newRow("invalid.pair") << qMetaTypeId<QPair<QString, int>>()
								  << QJsonTypeConverter::NoTag
								  << QCborValue::Map
								  << false
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << qMetaTypeId<QMap<QString, int>>()
								 << static_cast<QCborTag>(QCborSerializer::GenericObject)
								 << QCborValue::Map
								 << true
								 << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
}

void MapConverterTest::addCommonSerData()
{
	QTest::newRow("empty") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<QMap<QString, int>>()
						   << QVariant::fromValue(QMap<QString, int>{})
						   << QCborValue{QCborMap{}}
						   << QJsonValue{QJsonObject{}};
	QTest::newRow("filled.map") << QVariantHash{}
								<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
								<< static_cast<QObject*>(this)
								<< qMetaTypeId<QMap<QString, int>>()
								<< QVariant::fromValue(QMap<QString, int>{
										  {QStringLiteral("a"), 1},
										  {QStringLiteral("b"), 3},
										  {QStringLiteral("c"), 5}
									  })
								<< QCborValue{QCborMap{
										{QStringLiteral("a"), 2},
										{QStringLiteral("b"), 4},
										{QStringLiteral("c"), 6}
									}}
								<< QJsonValue{QJsonObject{
										{QStringLiteral("a"), 2},
										{QStringLiteral("b"), 4},
										{QStringLiteral("c"), 6}
									}};
	QTest::newRow("filled.hash") << QVariantHash{}
								 << TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<QHash<QString, int>>()
								 << QVariant::fromValue(QHash<QString, int>{
										   {QStringLiteral("a"), 1},
										   {QStringLiteral("b"), 3},
										   {QStringLiteral("c"), 5}
									   })
								 << QCborValue{QCborMap{
										{QStringLiteral("a"), 2},
										{QStringLiteral("b"), 4},
										{QStringLiteral("c"), 6}
									}}
								 << QJsonValue{QJsonObject{
										 {QStringLiteral("a"), 2},
										 {QStringLiteral("b"), 4},
										 {QStringLiteral("c"), 6}
									 }};
	QTest::newRow("variant.map") << QVariantHash{}
								 << TestQ{{QMetaType::UnknownType, true, false}}
								 << static_cast<QObject*>(this)
								 << static_cast<int>(QMetaType::QVariantMap)
								 << QVariant{QVariantMap{{QStringLiteral("tree"), true}}}
								 << QCborValue{QCborMap{{QStringLiteral("tree"), false}}}
								 << QJsonValue{QJsonObject{{QStringLiteral("tree"), false}}};
	QTest::newRow("variant.hash") << QVariantHash{}
								  << TestQ{{QMetaType::UnknownType, true, false}}
								  << static_cast<QObject*>(this)
								  << static_cast<int>(QMetaType::QVariantHash)
								  << QVariant{QVariantHash{{QStringLiteral("tree"), true}}}
								  << QCborValue{QCborMap{{QStringLiteral("tree"), false}}}
								  << QJsonValue{QJsonObject{{QStringLiteral("tree"), false}}};
}

void MapConverterTest::addSerData()
{
	QTest::newRow("unconvertible") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<QMap<QString, OpaqueDummy>>()
								   << QVariant::fromValue(QMap<QString, OpaqueDummy>{{QStringLiteral("d"), {}}})
								   << QCborValue{}
								   << QJsonValue{QJsonValue::Undefined};
}

QTEST_MAIN(MapConverterTest)

#include "tst_mapconverter.moc"
