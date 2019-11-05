#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/mapconverter_p.h>
using namespace QtJsonSerializer;
using namespace QtJsonSerializer::TypeConverters;

class MapConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	TypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	MapConverter _converter;
};

void MapConverterTest::initTest()
{
	SerializerBase::registerMapConverters<int, double>();
	SerializerBase::registerMapConverters<QPair<int, int>, double>();
	SerializerBase::registerMapConverters<QString, QList<int>>();
	SerializerBase::registerMapConverters<QPair<int, int>, QPair<int, bool>>();

	QMetaType::registerEqualsComparator<QMap<QString, int>>();
	QMetaType::registerEqualsComparator<QHash<QString, bool>>();
	QMetaType::registerEqualsComparator<QMap<int, double>>();
	QMetaType::registerEqualsComparator<QHash<int, double>>();
	QMetaType::registerEqualsComparator<QMap<QPair<int, int>, double>>();
}

TypeConverter *MapConverterTest::converter()
{
	return &_converter;
}

void MapConverterTest::addConverterData()
{
	QTest::newRow("map") << static_cast<int>(TypeConverter::Standard);
}

void MapConverterTest::addMetaData()
{
	QTest::newRow("int") << qMetaTypeId<QMap<QString, int>>()
						 << static_cast<QCborTag>(CborSerializer::NoTag)
						 << QCborValue::Map
						 << true
						 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("string") << qMetaTypeId<QMap<QString, QString>>()
							<< static_cast<QCborTag>(CborSerializer::ExplicitMap)
							<< QCborValue::Map
							<< true
							<< TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("key") << qMetaTypeId<QMap<int, double>>()
						 << static_cast<QCborTag>(CborSerializer::ExplicitMap)
						 << QCborValue::Map
						 << true
						 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("variant") << static_cast<int>(QMetaType::QVariantMap)
							 << static_cast<QCborTag>(CborSerializer::NoTag)
							 << QCborValue::Map
							 << true
							 << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("list") << qMetaTypeId<QMap<QString, QList<int>>>()
						  << static_cast<QCborTag>(CborSerializer::NoTag)
						  << QCborValue::Map
						  << true
						  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("complex") << qMetaTypeId<QMap<QPair<int, int>, QPair<int, bool>>>()
							 << static_cast<QCborTag>(CborSerializer::NoTag)
							 << QCborValue::Map
							 << true
							 << TypeConverter::DeserializationCapabilityResult::Positive;

	QTest::newRow("hash") << qMetaTypeId<QHash<QString, int>>()
						  << static_cast<QCborTag>(CborSerializer::NoTag)
						  << QCborValue::Map
						  << true
						  << TypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("hash.key") << qMetaTypeId<QHash<int, double>>()
							  << static_cast<QCborTag>(CborSerializer::NoTag)
							  << QCborValue::Map
							  << true
							  << TypeConverter::DeserializationCapabilityResult::Positive;

	QTest::newRow("invalid.pair") << qMetaTypeId<QPair<QString, int>>()
								  << static_cast<QCborTag>(CborSerializer::NoTag)
								  << QCborValue::Map
								  << false
								  << TypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << qMetaTypeId<QMap<QString, int>>()
								 << static_cast<QCborTag>(CborSerializer::GenericObject)
								 << QCborValue::Map
								 << true
								 << TypeConverter::DeserializationCapabilityResult::WrongTag;
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
										{QMetaType::UnknownType, 1, 2},
										{QMetaType::UnknownType, 3, 4},
										{QMetaType::UnknownType, 5, 6}
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
										 {QMetaType::UnknownType, true, false},
									 }
								  << static_cast<QObject*>(this)
								  << static_cast<int>(QMetaType::QVariantHash)
								  << QVariant{QVariantHash{{QStringLiteral("tree"), true}}}
								  << QCborValue{QCborMap{{QStringLiteral("baum"), false}}}
								  << QJsonValue{QJsonObject{{QStringLiteral("baum"), false}}};

	QTest::newRow("complex") << QVariantHash{}
							 << TestQ{
									{qMetaTypeId<QPair<int, int>>(), QVariant::fromValue<QPair<int, int>>({1, 2}), QCborArray{12, 21}},
									{qMetaTypeId<QPair<int, int>>(), QVariant::fromValue<QPair<int, int>>({4, 5}), QCborArray{45, 54}},
									{qMetaTypeId<QPair<int, int>>(), QVariant::fromValue<QPair<int, int>>({7, 8}), QCborArray{78, 87}},
									{QMetaType::Double, 3.3, 33.33},
									{QMetaType::Double, 6.6, 66.66},
									{QMetaType::Double, 9.9, 99.99}
								}
							 << static_cast<QObject*>(this)
							 << qMetaTypeId<QMap<QPair<int, int>, double>>()
							 << QVariant::fromValue(QMap<QPair<int, int>, double>{
									{{1, 2}, 3.3},
									{{4, 5}, 6.6},
									{{7, 8}, 9.9},
								})
							 << QCborValue{QCborMap{
									{QCborArray{12, 21}, 33.33},
									{QCborArray{45, 54}, 66.66},
									{QCborArray{78, 87}, 99.99},
								}}
							 << QJsonValue{QJsonValue::Undefined};
}

void MapConverterTest::addDeserData()
{
	QTest::newRow("unwritable") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< qMetaTypeId<QMap<QString, OpaqueDummy>>()
								<< QVariant{}
								<< QCborValue{QCborMap{}}
								<< QJsonValue{QJsonObject{}};
}

QTEST_MAIN(MapConverterTest)

#include "tst_mapconverter.moc"
