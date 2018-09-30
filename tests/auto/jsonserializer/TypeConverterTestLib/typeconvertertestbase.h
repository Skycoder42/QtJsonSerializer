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
	virtual void addCommonSerData() = 0;
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

	void testConverterMeta_data();
	void testConverterMeta();

	void testMetaTypeDetection_data();
	void testMetaTypeDetection();

	void testSerialization_data();
	void testSerialization();

	void testDeserialization_data();
	void testDeserialization();
};

Q_DECLARE_METATYPE(QJsonValue::Type)

#endif // TYPECONVERTERTESTLIB_H
