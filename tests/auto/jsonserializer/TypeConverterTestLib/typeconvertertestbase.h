#ifndef TYPECONVERTERTESTLIB_H
#define TYPECONVERTERTESTLIB_H

#include <QtCore/QObject>
#include <QtCore/QJsonValue>
#include <QtJsonSerializer/QJsonTypeConverter>

#include "dummyserializationhelper.h"
#include "opaquedummy.h"

class TypeConverterTestBase : public QObject
{
	Q_OBJECT
	friend class MultiTypeConverterTestBase;

public:
	using TestQ = QList<DummySerializationHelper::SerInfo>;

	TypeConverterTestBase(QObject *parent = nullptr);

protected:
	DummySerializationHelper * const helper;

	virtual QJsonTypeConverter *converter() = 0;

	virtual void initTest();
	virtual void cleanupTest();

	virtual void addConverterData() = 0;
	virtual void addMetaData() = 0;
	virtual void addCommonSerData();
	virtual void addSerData();
	virtual void addDeserData();

	virtual bool compare(int type,
						 QVariant &actual,
						 QVariant &expected,
						 const char *aName,
						 const char *eName,
						 const char *file,
						 int line);

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	virtual void testConverterIsRegistered_data();
	void testConverterIsRegistered();
	virtual void testConverterMeta_data();
	void testConverterMeta();
	virtual void testMetaTypeDetection_data();
	void testMetaTypeDetection();
	virtual void testSerialization_data();
	void testSerialization();
	virtual void testDeserialization_data();
	void testDeserialization();
};

Q_DECLARE_METATYPE(QCborValue::Type)
Q_DECLARE_METATYPE(QJsonValue::Type)
Q_DECLARE_METATYPE(QJsonTypeConverter::DeserializationCapabilityResult)

#endif // TYPECONVERTERTESTLIB_H
