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
	QJsonSerializerBase::registerMapConverters<int, double>();

	QMetaType::registerEqualsComparator<QMap<QString, int>>();
	QMetaType::registerEqualsComparator<QHash<QString, bool>>();
	QMetaType::registerEqualsComparator<QMap<int, double>>();
	QMetaType::registerEqualsComparator<QHash<int, double>>();
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
	QTest::newRow("key") << qMetaTypeId<QMap<int, double>>()
						 << static_cast<QCborTag>(QCborSerializer::ExplicitMap)
						 << QCborValue::Map
						 << true
						 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("variant") << static_cast<int>(QMetaType::QVariantMap)
							 << QJsonTypeConverter::NoTag
							 << QCborValue::Map
							 << true
							 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("list") << qMetaTypeId<QMap<QString, QList<int>>>()
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
	QTest::newRow("hash.key") << qMetaTypeId<QHash<int, double>>()
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
								<< TestQ{
									   {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
									   {QMetaType::QString, QStringLiteral("b"), QStringLiteral("B")},
									   {QMetaType::QString, QStringLiteral("c"), QStringLiteral("C")},
									   {QMetaType::Int, 1, 2},
									   {QMetaType::Int, 3, 4},
									   {QMetaType::Int, 5, 6}
								   }
								<< static_cast<QObject*>(this)
								<< qMetaTypeId<QMap<QString, int>>()
								<< QVariant::fromValue(QMap<QString, int>{
										  {QStringLiteral("a"), 1},
										  {QStringLiteral("b"), 3},
										  {QStringLiteral("c"), 5}
									  })
								<< QCborValue{QCborMap{
										{QStringLiteral("A"), 2},
										{QStringLiteral("B"), 4},
										{QStringLiteral("C"), 6}
									}}
								<< QJsonValue{QJsonObject{
										{QStringLiteral("A"), 2},
										{QStringLiteral("B"), 4},
										{QStringLiteral("C"), 6}
									}};
	QTest::newRow("filled.hash") << QVariantHash{}
								 << TestQ{
										{QMetaType::QString, QStringLiteral("tree"), QStringLiteral("baum")},
										{QMetaType::Bool, true, false},
									}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<QHash<QString, bool>>()
								 << QVariant::fromValue(QHash<QString, bool>{{QStringLiteral("tree"), true}})
								 << QCborValue{QCborMap{{QStringLiteral("baum"), false}}}
								 << QJsonValue{QJsonObject{{QStringLiteral("baum"), false}}};
	QTest::newRow("key.map") << QVariantHash{}
							 << TestQ{
									{QMetaType::Int, 1, 2},
									{QMetaType::Int, 3, 4},
									{QMetaType::Int, 5, 6},
									{QMetaType::Double, 0.1, 0.2},
									{QMetaType::Double, 0.3, 0.4},
									{QMetaType::Double, 0.5, 0.6}
								}
							 << static_cast<QObject*>(this)
							 << qMetaTypeId<QMap<int, double>>()
							 << QVariant::fromValue(QMap<int, double>{
									{1, 0.1},
									{3, 0.3},
									{5, 0.5}
								})
							 << QCborValue{QCborMap{
									{2, 0.2},
									{4, 0.4},
									{6, 0.6}
								}}
							 << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("key.hash") << QVariantHash{}
							  << TestQ{
									 {QMetaType::Int, 1, 2},
									 {QMetaType::Double, 0.3, 0.4},
								 }
							  << static_cast<QObject*>(this)
							  << qMetaTypeId<QHash<int, double>>()
							  << QVariant::fromValue(QHash<int, double>{{1, 0.3}})
							  << QCborValue{QCborMap{{2, 0.4}}}
							  << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("variant.map") << QVariantHash{}
								 << TestQ{
										{QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
										{QMetaType::QString, QStringLiteral("b"), QStringLiteral("B")},
										{QMetaType::QString, QStringLiteral("c"), QStringLiteral("C")},
										{QMetaType::QVariant, 1, 2},
										{QMetaType::QVariant, 3, 4},
										{QMetaType::QVariant, 5, 6}
									}
								 << static_cast<QObject*>(this)
								 << static_cast<int>(QMetaType::QVariantMap)
								 << QVariant{QVariantMap{
										{QStringLiteral("a"), 1},
										{QStringLiteral("b"), 3},
										{QStringLiteral("c"), 5}
									}}
								 << QCborValue{QCborMap{
										{QStringLiteral("A"), 2},
										{QStringLiteral("B"), 4},
										{QStringLiteral("C"), 6}
									}}
								 << QJsonValue{QJsonObject{
										{QStringLiteral("A"), 2},
										{QStringLiteral("B"), 4},
										{QStringLiteral("C"), 6}
									}};
	QTest::newRow("variant.hash") << QVariantHash{}
								  << TestQ{
										 {QMetaType::QString, QStringLiteral("tree"), QStringLiteral("baum")},
										 {QMetaType::QVariant, true, false},
									 }
								  << static_cast<QObject*>(this)
								  << static_cast<int>(QMetaType::QVariantHash)
								  << QVariant{QVariantHash{{QStringLiteral("tree"), true}}}
								  << QCborValue{QCborMap{{QStringLiteral("baum"), false}}}
								  << QJsonValue{QJsonObject{{QStringLiteral("baum"), false}}};
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
