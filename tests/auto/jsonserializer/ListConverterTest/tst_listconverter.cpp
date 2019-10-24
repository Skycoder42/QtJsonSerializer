#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonlistconverter_p.h>

class ListConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addDeserData() override;

private:
	QJsonListConverter _converter;
};

void ListConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<QList<int>>();
	QMetaType::registerEqualsComparator<QLinkedList<int>>();
	QMetaType::registerEqualsComparator<QVector<int>>();
	QMetaType::registerEqualsComparator<QStack<int>>();
	QMetaType::registerEqualsComparator<QQueue<int>>();
	QMetaType::registerEqualsComparator<QSet<int>>();
}

QJsonTypeConverter *ListConverterTest::converter()
{
	return &_converter;
}

void ListConverterTest::addConverterData()
{
	QTest::newRow("list") << static_cast<int>(QJsonTypeConverter::Standard);
}

void ListConverterTest::addMetaData()
{
	QTest::newRow("int") << qMetaTypeId<QList<int>>()
						 << static_cast<QCborTag>(QCborSerializer::Homogeneous)
						 << QCborValue::Array
						 << true
						 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("string") << static_cast<int>(QMetaType::QStringList)
							<< static_cast<QCborTag>(QCborSerializer::Homogeneous)
							<< QCborValue::Array
							<< true
							<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("bytearray") << static_cast<int>(QMetaType::QByteArrayList)
							   << static_cast<QCborTag>(QCborSerializer::Homogeneous)
							   << QCborValue::Array
							   << true
							   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("variant") << static_cast<int>(QMetaType::QVariantList)
							 << static_cast<QCborTag>(QCborSerializer::Homogeneous)
							 << QCborValue::Array
							 << true
							 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("list") << qMetaTypeId<QList<QList<bool>>>()
						  << static_cast<QCborTag>(QCborSerializer::Homogeneous)
						  << QCborValue::Array
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("pair") << qMetaTypeId<QList<QPair<int, bool>>>()
						  << static_cast<QCborTag>(QCborSerializer::Homogeneous)
						  << QCborValue::Array
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("untagged") << qMetaTypeId<QList<int>>()
							  << static_cast<QCborTag>(QCborSerializer::NoTag)
							  << QCborValue::Array
							  << true
							  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("invalid") << static_cast<int>(QMetaType::UnknownType)
							 << static_cast<QCborTag>(QCborSerializer::Homogeneous)
							 << QCborValue::Array
							 << false
							 << QJsonTypeConverter::DeserializationCapabilityResult::Negative;

	QTest::newRow("linkedList") << qMetaTypeId<QLinkedList<int>>()
								<< static_cast<QCborTag>(QCborSerializer::Homogeneous)
								<< QCborValue::Array
								<< true
								<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("vector") << qMetaTypeId<QVector<int>>()
							<< static_cast<QCborTag>(QCborSerializer::Homogeneous)
							<< QCborValue::Array
							<< true
							<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("stack") << qMetaTypeId<QStack<int>>()
						   << static_cast<QCborTag>(QCborSerializer::Homogeneous)
						   << QCborValue::Array
						   << true
						   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("queue") << qMetaTypeId<QQueue<int>>()
						   << static_cast<QCborTag>(QCborSerializer::Homogeneous)
						   << QCborValue::Array
						   << true
						   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("set.set") << qMetaTypeId<QSet<int>>()
							 << static_cast<QCborTag>(QCborSerializer::Set)
							 << QCborValue::Array
							 << true
							 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("set.homogeneous") << qMetaTypeId<QSet<int>>()
									 << static_cast<QCborTag>(QCborSerializer::Homogeneous)
									 << QCborValue::Array
									 << true
									 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
}

void ListConverterTest::addCommonSerData()
{
	QTest::newRow("empty") << QVariantHash{}
						   << TestQ{}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<QList<int>>()
						   << QVariant::fromValue(QList<int>{})
						   << QCborValue{QCborArray{}}
						   << QJsonValue{QJsonArray{}};
	QTest::newRow("filled") << QVariantHash{}
							<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							<< static_cast<QObject*>(this)
							<< qMetaTypeId<QList<int>>()
							<< QVariant::fromValue(QList<int>{1, 3, 5})
							<< QCborValue{QCborArray{2, 4, 6}}
							<< QJsonValue{QJsonArray{2, 4, 6}};
	QTest::newRow("string") << QVariantHash{}
							<< TestQ{{QMetaType::QString, QStringLiteral("test"), QStringLiteral("tree")}}
							<< static_cast<QObject*>(this)
							<< static_cast<int>(QMetaType::QStringList)
							<< QVariant{QStringList{QStringLiteral("test")}}
							<< QCborValue{QCborArray{QStringLiteral("tree")}}
							<< QJsonValue{QJsonArray{QStringLiteral("tree")}};
	QTest::newRow("bytearray") << QVariantHash{}
							   << TestQ{{QMetaType::QByteArray, QByteArray("test"), QByteArray("tree")}}
							   << static_cast<QObject*>(this)
							   << static_cast<int>(QMetaType::QByteArrayList)
							   << QVariant::fromValue(QByteArrayList{"test"})
							   << QCborValue{QCborArray{QByteArray("tree")}}
							   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("variant") << QVariantHash{}
							 << TestQ{{QMetaType::UnknownType, true, false}}
							 << static_cast<QObject*>(this)
							 << static_cast<int>(QMetaType::QVariantList)
							 << QVariant{QVariantList{true}}
							 << QCborValue{QCborArray{false}}
							 << QJsonValue{QJsonArray{false}};

	QTest::newRow("linkedList") << QVariantHash{}
								<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
								<< static_cast<QObject*>(this)
								<< qMetaTypeId<QLinkedList<int>>()
								<< QVariant::fromValue(QLinkedList<int>{1, 3, 5})
								<< QCborValue{QCborArray{2, 4, 6}}
								<< QJsonValue{QJsonArray{2, 4, 6}};
	QTest::newRow("vector") << QVariantHash{}
							<< TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							<< static_cast<QObject*>(this)
							<< qMetaTypeId<QVector<int>>()
							<< QVariant::fromValue(QVector<int>{1, 3, 5})
							<< QCborValue{QCborArray{2, 4, 6}}
							<< QJsonValue{QJsonArray{2, 4, 6}};

	{
		QStack<int> s;
		s.push(1);
		s.push(3);
		s.push(5);
		QTest::newRow("stack") << QVariantHash{}
							   << TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							   << static_cast<QObject*>(this)
							   << qMetaTypeId<QStack<int>>()
							   << QVariant::fromValue(s)
							   << QCborValue{QCborArray{2, 4, 6}}
							   << QJsonValue{QJsonArray{2, 4, 6}};
	}

	{
		QQueue<int> q;
		q.enqueue(1);
		q.enqueue(3);
		q.enqueue(5);
		QTest::newRow("queue") << QVariantHash{}
							   << TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
							   << static_cast<QObject*>(this)
							   << qMetaTypeId<QQueue<int>>()
							   << QVariant::fromValue(q)
							   << QCborValue{QCborArray{2, 4, 6}}
							   << QJsonValue{QJsonArray{2, 4, 6}};
	}

	{
		QSet<int> s{1, 3, 5};
		TestQ q;
		auto ctr = 0;
		for(const auto &v : qAsConst(s))
			q.append({QMetaType::Int, v, (++ctr)*2});
		QTest::newRow("set") << QVariantHash{}
							 << q
							 << static_cast<QObject*>(this)
							 << qMetaTypeId<QSet<int>>()
							 << QVariant::fromValue(s)
							 << QCborValue{static_cast<QCborTag>(QCborSerializer::Set), QCborArray{2, 4, 6}}
							 << QJsonValue{QJsonArray{2, 4, 6}};
	}
}

void ListConverterTest::addDeserData()
{
	QTest::newRow("homogeneous.list") << QVariantHash{}
									  << TestQ{{QMetaType::Int, 1, 2}, {QMetaType::Int, 3, 4}, {QMetaType::Int, 5, 6}}
									  << static_cast<QObject*>(this)
									  << qMetaTypeId<QList<int>>()
									  << QVariant::fromValue(QList<int>{1, 3, 5})
									  << QCborValue{static_cast<QCborTag>(QCborSerializer::Homogeneous), QCborArray{2, 4, 6}}
									  << QJsonValue{QJsonArray{2, 4, 6}};
	QSet<int> s{1, 3, 5};
	TestQ q;
	auto ctr = 0;
	for(const auto &v : qAsConst(s))
		q.append({QMetaType::Int, v, (++ctr)*2});
	QTest::newRow("homogeneous.set") << QVariantHash{}
									<< q
									<< static_cast<QObject*>(this)
									<< qMetaTypeId<QSet<int>>()
									<< QVariant::fromValue(s)
									<< QCborValue{static_cast<QCborTag>(QCborSerializer::Homogeneous), QCborArray{2, 4, 6}}
									<< QJsonValue{QJsonArray{2, 4, 6}};
	QTest::newRow("unwritable") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< qMetaTypeId<QList<OpaqueDummy>>()
								<< QVariant{}
								<< QCborValue{QCborArray{}}
								<< QJsonValue{QJsonArray{}};
}

QTEST_MAIN(ListConverterTest)

#include "tst_listconverter.moc"
