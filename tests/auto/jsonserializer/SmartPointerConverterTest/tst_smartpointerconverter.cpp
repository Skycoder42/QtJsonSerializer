#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonsmartpointerconverter_p.h>

#include "testobject.h"

class SmartPointerConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addSerData() override;
	void addDeserData() override;
	bool compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line) override;

private:
	QJsonSmartPointerConverter _converter;
};

void SmartPointerConverterTest::initTest()
{
	qRegisterMetaType<TestClass*>();
	qRegisterMetaType<QSharedPointer<TestClass>>();

	QJsonSerializerBase::registerPointerConverters<TestClass>();
	QJsonSerializerBase::registerPointerConverters<TestObject>();
}

QJsonTypeConverter *SmartPointerConverterTest::converter()
{
	return &_converter;
}

void SmartPointerConverterTest::addConverterData()
{
	QTest::newRow("smart") << static_cast<int>(QJsonTypeConverter::Standard);
}

void SmartPointerConverterTest::addMetaData()
{
	QTest::newRow("class.shared") << qMetaTypeId<QSharedPointer<TestClass>>()
								  << static_cast<QCborTag>(QCborSerializer::NoTag)
								  << QCborValue::Map
								  << true
								  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("object.shared") << qMetaTypeId<QSharedPointer<TestObject>>()
								   << static_cast<QCborTag>(QCborSerializer::NoTag)
								   << QCborValue::Map
								   << true
								   << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("object.ptr") << qMetaTypeId<QPointer<TestObject>>()
								<< static_cast<QCborTag>(QCborSerializer::NoTag)
								<< QCborValue::Map
								<< true
								<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;

	QTest::newRow("invalid.raw") << qMetaTypeId<TestClass*>()
								 << static_cast<QCborTag>(QCborSerializer::NoTag)
								 << QCborValue::Map
								 << false
								 << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
}

void SmartPointerConverterTest::addSerData()
{
	auto tc0 = QSharedPointer<TestClass>::create(42);
	QTest::newRow("class.shared") << QVariantHash{}
								  << TestQ{{qMetaTypeId<TestClass*>(), QVariant::fromValue(tc0.data()), 42}}
								  << static_cast<QObject*>(nullptr)
								  << qMetaTypeId<QSharedPointer<TestClass>>()
								  << QVariant::fromValue(tc0)
								  << QCborValue{42}
								  << QJsonValue{42};

	auto to0 = QSharedPointer<TestObject>::create(13);
	QTest::newRow("obj.shared") << QVariantHash{}
								<< TestQ{{qMetaTypeId<TestObject*>(), QVariant::fromValue(to0.data()), 13}}
								<< static_cast<QObject*>(nullptr)
								<< qMetaTypeId<QSharedPointer<TestObject>>()
								<< QVariant::fromValue(to0)
								<< QCborValue{13}
								<< QJsonValue{13};
	QPointer<TestObject> to1 = new TestObject{11, this};
	QTest::newRow("obj.shared") << QVariantHash{}
								<< TestQ{{qMetaTypeId<TestObject*>(), QVariant::fromValue(to1.data()), 11}}
								<< static_cast<QObject*>(this)
								<< qMetaTypeId<QPointer<TestObject>>()
								<< QVariant::fromValue(to1)
								<< QCborValue{11}
								<< QJsonValue{11};
}

void SmartPointerConverterTest::addDeserData()
{
	QTest::newRow("class.shared.cbor") << QVariantHash{}
									   << TestQ{{qMetaTypeId<TestClass*>(), QVariant::fromValue(new TestClass{42}), 42}}
									   << static_cast<QObject*>(nullptr)
									   << qMetaTypeId<QSharedPointer<TestClass>>()
									   << QVariant::fromValue(QSharedPointer<TestClass>::create(42))
									   << QCborValue{42}
									   << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("class.shared.json") << QVariantHash{}
									   << TestQ{{qMetaTypeId<TestClass*>(), QVariant::fromValue(new TestClass{42}), 42}}
									   << static_cast<QObject*>(nullptr)
									   << qMetaTypeId<QSharedPointer<TestClass>>()
									   << QVariant::fromValue(QSharedPointer<TestClass>::create(42))
									   << QCborValue{}
									   << QJsonValue{42};

	QTest::newRow("obj.shared.cbor") << QVariantHash{}
									 << TestQ{{qMetaTypeId<TestObject*>(), QVariant::fromValue(new TestObject{13}), 13}}
									 << static_cast<QObject*>(nullptr)
									 << qMetaTypeId<QSharedPointer<TestObject>>()
									 << QVariant::fromValue(QSharedPointer<TestObject>::create(13))
									 << QCborValue{13}
									 << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("obj.shared.json") << QVariantHash{}
									 << TestQ{{qMetaTypeId<TestObject*>(), QVariant::fromValue(new TestObject{13}), 13}}
									 << static_cast<QObject*>(nullptr)
									 << qMetaTypeId<QSharedPointer<TestObject>>()
									 << QVariant::fromValue(QSharedPointer<TestObject>::create(13))
									 << QCborValue{}
									 << QJsonValue{13};
	QTest::newRow("obj.shared.cbor") << QVariantHash{}
									 << TestQ{{qMetaTypeId<TestObject*>(), QVariant::fromValue(new TestObject{11, this}), 11}}
									 << static_cast<QObject*>(this)
									 << qMetaTypeId<QPointer<TestObject>>()
									 << QVariant::fromValue<QPointer<TestObject>>(new TestObject{11, this})
									 << QCborValue{11}
									 << QJsonValue{QJsonValue::Undefined};
	QTest::newRow("obj.shared.json") << QVariantHash{}
									 << TestQ{{qMetaTypeId<TestObject*>(), QVariant::fromValue(new TestObject{11, this}), 11}}
									 << static_cast<QObject*>(this)
									 << qMetaTypeId<QPointer<TestObject>>()
									 << QVariant::fromValue<QPointer<TestObject>>(new TestObject{11, this})
									 << QCborValue{}
									 << QJsonValue{11};
}

bool SmartPointerConverterTest::compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line)
{
	if (type == qMetaTypeId<QSharedPointer<TestClass>>()) {
		auto va = QVariant::fromValue(actual.value<QSharedPointer<TestClass>>().data());
		auto ve = QVariant::fromValue(expected.value<QSharedPointer<TestClass>>().data());
		return compare(qMetaTypeId<TestClass*>(), va, ve, aName, eName, file, line);
	} else if (type == qMetaTypeId<TestClass*>()) {
		const auto obj1 = actual.value<TestClass*>();
		const auto obj2 = expected.value<TestClass*>();
		if(obj1 != obj2) { //same object is automatically equal
			if(!QTest::qVerify(obj1, aName, "object pointer verification", file, line))
				return false;
			if(!QTest::qVerify(obj2, eName, "object pointer verification", file, line))
				return false;
			return QTest::qCompare(obj1->value, obj2->value, aName, eName, file, line);
		} else
			return true;
	} else if (type == qMetaTypeId<QSharedPointer<TestObject>>()) {
		auto va = QVariant::fromValue(actual.value<QSharedPointer<TestObject>>().data());
		auto ve = QVariant::fromValue(expected.value<QSharedPointer<TestObject>>().data());
		return compare(qMetaTypeId<TestObject*>(), va, ve, aName, eName, file, line);
	} else if (type == qMetaTypeId<QPointer<TestObject>>()) {
		auto va = QVariant::fromValue(actual.value<QPointer<TestObject>>().data());
		auto ve = QVariant::fromValue(expected.value<QPointer<TestObject>>().data());
		return compare(qMetaTypeId<TestObject*>(), va, ve, aName, eName, file, line);
	} else if (type == qMetaTypeId<TestObject*>()) {
		const auto obj1 = actual.value<TestObject*>();
		const auto obj2 = expected.value<TestObject*>();
		if(obj1 != obj2) { //same object is automatically equal
			if(!QTest::qVerify(obj1, aName, "object pointer verification", file, line))
				return false;
			if(!QTest::qVerify(obj2, eName, "object pointer verification", file, line))
				return false;
			return QTest::qCompare(obj1->value, obj2->value, aName, eName, file, line);
		} else
			return true;
	} else
		return TypeConverterTestBase::compare(type, actual, expected, aName, eName, file, line);
}

QTEST_MAIN(SmartPointerConverterTest)

#include "tst_smartpointerconverter.moc"
