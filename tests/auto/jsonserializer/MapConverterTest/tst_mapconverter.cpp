#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonmapconverter_p.h>

template <typename T>
bool operator <(const QMap<QString, T> &m1, const QMap<QString, T> &m2)
{
	return m1.keys() < m2.keys() &&
			m1.values() < m2.values();
}

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
}

QJsonTypeConverter *MapConverterTest::converter()
{
	return &_converter;
}

void MapConverterTest::addConverterData()
{

	QTest::newRow("map") << static_cast<int>(QJsonTypeConverter::Standard)
						 << QList<QJsonValue::Type>{QJsonValue::Object};
}

void MapConverterTest::addMetaData()
{
	QTest::newRow("int") << qMetaTypeId<QMap<QString, int>>()
						 << true;
	QTest::newRow("string") << qMetaTypeId<QMap<QString, QString>>()
							<< true;
	QTest::newRow("variant") << static_cast<int>(QMetaType::QVariantMap)
							 << true;
	QTest::newRow("object") << qMetaTypeId<QMap<QString, QObject*>>()
							<< true;
	QTest::newRow("list") << qMetaTypeId<QMap<QString, QMap<QString, bool>>>()
						  << true;
	QTest::newRow("pair") << qMetaTypeId<QMap<QString, QPair<int, bool>>>()
						  << true;
	QTest::newRow("invalid.hash") << qMetaTypeId<QHash<QString, int>>()
								  << false;
	QTest::newRow("invalid.key") << qMetaTypeId<QMap<int, int>>()
								 << false;
}

void MapConverterTest::addCommonSerData()
{
	QTest::newRow("empty") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<QMap<QString, int>>()
						   << QVariant::fromValue(QMap<QString, int>{})
						   << QJsonValue{QJsonObject{}};
	QTest::newRow("filled") << QVariantHash{}
							<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							<< static_cast<QObject*>(this)
							<< qMetaTypeId<QMap<QString, int>>()
							<< QVariant::fromValue(QMap<QString, int>{
									  {QStringLiteral("a"), 1},
									  {QStringLiteral("b"), 3},
									  {QStringLiteral("c"), 5}
								  })
							<< QJsonValue{QJsonObject{
									{QStringLiteral("a"), 2},
									{QStringLiteral("b"), 4},
									{QStringLiteral("c"), 6}
								}};
	QTest::newRow("variant") << QVariantHash{}
							 << TestQ{{QMetaType::UnknownType, true, false}}
							 << static_cast<QObject*>(this)
							 << static_cast<int>(QMetaType::QVariantMap)
							 << QVariant{QVariantMap{{QStringLiteral("tree"), true}}}
							 << QJsonValue{QJsonObject{{QStringLiteral("tree"), false}}};
}

void MapConverterTest::addSerData()
{
	QTest::newRow("unconvertible") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<QMap<QString, OpaqueDummy>>()
								   << QVariant::fromValue(QMap<QString, OpaqueDummy>{{QStringLiteral("d"), {}}})
								   << QJsonValue{QJsonValue::Undefined};
}

QTEST_MAIN(MapConverterTest)

#include "tst_mapconverter.moc"
