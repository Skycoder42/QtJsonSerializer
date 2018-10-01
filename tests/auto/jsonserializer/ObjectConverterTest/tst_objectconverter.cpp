#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"
#include "testobject.h"

#include <QtJsonSerializer/private/qjsonobjectconverter_p.h>

class ObjectConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;

	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;
	void addDeserData() override;

	bool compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line) override;

private:
	QJsonObjectConverter _converter;
};

void ObjectConverterTest::initTest()
{
	QJsonSerializer::registerPointerConverters<TestObject>();

}

QJsonTypeConverter *ObjectConverterTest::converter()
{
	return &_converter;
}

void ObjectConverterTest::addConverterData()
{
	QTest::newRow("object") << static_cast<int>(QJsonTypeConverter::Standard)
							<< QList<QJsonValue::Type>{QJsonValue::Object, QJsonValue::Null};
}

void ObjectConverterTest::addMetaData()
{
	QTest::newRow("basic") << qMetaTypeId<TestObject*>()
						   << true;
	QTest::newRow("base") << static_cast<int>(QMetaType::QObjectStar)
						  << true;
	QTest::newRow("tracking") << qMetaTypeId<QPointer<TestObject>>()
							  << true;
	QTest::newRow("shared") << qMetaTypeId<QSharedPointer<TestObject>>()
							<< true;
	QTest::newRow("invalid1") << qMetaTypeId<OpaqueDummyGadget*>()
							  << false;
	QTest::newRow("invalid2") << qMetaTypeId<OpaqueDummyGadget>()
							  << false;
}

void ObjectConverterTest::addCommonSerData()
{
	QTest::newRow("basic") << QVariantHash{}
						   << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
						   << static_cast<QObject*>(nullptr)
						   << qMetaTypeId<TestObject*>()
						   << QVariant::fromValue(new TestObject{10, 0.1, 11, this})
						   << QJsonValue{QJsonObject{
									{QStringLiteral("key"), 1},
									{QStringLiteral("value"), 2}
								}};
	QTest::newRow("tracking") << QVariantHash{}
							  << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
							  << static_cast<QObject*>(nullptr)
							  << qMetaTypeId<QPointer<TestObject>>()
							  << QVariant::fromValue<QPointer<TestObject>>(new TestObject{10, 0.1, 11, this})
							  << QJsonValue{QJsonObject{
										{QStringLiteral("key"), 1},
										{QStringLiteral("value"), 2}
									}};
	QTest::newRow("shared") << QVariantHash{}
							<< TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
							<< static_cast<QObject*>(nullptr)
							<< qMetaTypeId<QSharedPointer<TestObject>>()
							<< QVariant::fromValue(QSharedPointer<TestObject>::create(10, 0.1, 11))
							<< QJsonValue{QJsonObject{
									{QStringLiteral("key"), 1},
									{QStringLiteral("value"), 2}
								}};
	QTest::newRow("null.basic") << QVariantHash{}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< qMetaTypeId<TestObject*>()
								<< QVariant::fromValue<TestObject*>(nullptr)
								<< QJsonValue{QJsonValue::Null};
	QTest::newRow("null.tracking") << QVariantHash{}
								   << TestQ{}
								   << static_cast<QObject*>(nullptr)
								   << qMetaTypeId<QPointer<TestObject>>()
								   << QVariant::fromValue<QPointer<TestObject>>(nullptr)
								   << QJsonValue{QJsonValue::Null};
	QTest::newRow("null.shared") << QVariantHash{}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << qMetaTypeId<QSharedPointer<TestObject>>()
								 << QVariant::fromValue<QSharedPointer<TestObject>>(nullptr)
								 << QJsonValue{QJsonValue::Null};

//	QTest::newRow("poly.disabled") << QVariantHash{{QStringLiteral("polymorphing"), QVariant::fromValue<QJsonSerializer::Polymorphing>(QJsonSerializer::Disabled)}}
//										  << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}}
//										  << static_cast<QObject*>(nullptr)
//										  << qMetaTypeId<TestObject*>()
//										  << QVariant::fromValue(new TestObject{10, 0.1, 11, this})
//										  << QJsonValue{QJsonObject{
//													{QStringLiteral("key"), 1},
//													{QStringLiteral("value"), 2}
//												}};
//	QTest::newRow("poly.enabled.static") << QVariantHash{{QStringLiteral("polymorphing"), QVariant::fromValue<QJsonSerializer::Polymorphing>(QJsonSerializer::Enabled)}}
//										 << TestQ{{QMetaType::Int, 10, 1}, {QMetaType::Double, 0.1, 2}, {QMetaType::Bool, true, 3}}
//										 << static_cast<QObject*>(nullptr)
//										 << qMetaTypeId<TestObject*>()
//										 << QVariant::fromValue<TestObject*>(new StaticPolyObject{10, 0.1, 11, true, this})
//										 << QJsonValue{QJsonObject{
//												   {QStringLiteral("@class"), QStringLiteral("StaticPolyObject")},
//												   {QStringLiteral("key"), 1},
//												   {QStringLiteral("value"), 2},
//												   {QStringLiteral("extra"), 3}
//											   }};
}

void ObjectConverterTest::addSerData()
{

}

void ObjectConverterTest::addDeserData()
{

}

bool ObjectConverterTest::compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line)
{
	const auto flags = QMetaType::typeFlags(type);
	if(flags.testFlag(QMetaType::PointerToQObject) ||
	   flags.testFlag(QMetaType::TrackingPointerToQObject) ||
	   flags.testFlag(QMetaType::SharedPointerToQObject)) {
		const auto obj1 = actual.value<TestObject*>();
		const auto obj2 = expected.value<TestObject*>();
		if(obj1 != obj2) { //same object is automatically equal
			if(!QTest::qVerify(obj1, aName, "object pointer verification", file, line))
				return false;
			if(!QTest::qVerify(obj2, eName, "object pointer verification", file, line))
				return false;
			QByteArray statement {aName + QByteArrayLiteral("->compare(") + eName + QByteArrayLiteral(")")};
			return QTest::qVerify(obj1->compare(obj2), statement.constData(), "equality comparison", file, line);
		} else
			return true;
	} else
		return TypeConverterTestBase::compare(type, actual, expected, aName, eName, file, line);
}

QTEST_MAIN(ObjectConverterTest)

#include "tst_objectconverter.moc"
