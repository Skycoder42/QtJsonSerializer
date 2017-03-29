#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <QList>
#include <QFlags>

class TestObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int intProperty MEMBER intProperty)
	Q_PROPERTY(bool boolProperty MEMBER boolProperty)
	Q_PROPERTY(QString stringProperty MEMBER stringProperty)
	Q_PROPERTY(double doubleProperty MEMBER doubleProperty)

	Q_PROPERTY(NormalEnum normalEnumProperty MEMBER normalEnumProperty)
	Q_PROPERTY(EnumFlags enumFlagsProperty READ getEnumFlagsProperty WRITE setEnumFlagsProperty)

	Q_PROPERTY(QList<int> simpeList MEMBER simpeList)
	Q_PROPERTY(QList<QList<int>> leveledList MEMBER leveledList)

	Q_PROPERTY(TestObject* childObject MEMBER childObject)
	Q_PROPERTY(QList<TestObject*> simpleChildren MEMBER simpleChildren)
	Q_PROPERTY(QList<QList<TestObject*>> leveledChildren MEMBER leveledChildren)

public:
	enum NormalEnum {
		Normal0 = 0,
		Normal1 = 1,
		Normal2 = 2
	};
	Q_ENUM(NormalEnum)
	enum EnumFlag {
		Flag1 = 0x02,
		Flag2 = 0x04,

		FlagX = Flag1 | Flag2
	};
	Q_DECLARE_FLAGS(EnumFlags, EnumFlag)
	Q_FLAG(EnumFlags)

	Q_INVOKABLE TestObject(QObject *parent = nullptr);

	static TestObject *createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty, QObject *parent = nullptr);
	static TestObject *createEnum(NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty, QObject *parent = nullptr);
	static TestObject *createList(QList<int> simpeList, QList<QList<int>> leveledList, QObject *parent = nullptr);
	static TestObject *createChild(TestObject* childObject, QList<TestObject*> simpleChildren, QList<QList<TestObject*>> leveledChildren, QObject *parent = nullptr);

	static bool equals(const TestObject *left, const TestObject *right);
	bool equals(const TestObject *other) const;

	int intProperty;
	bool boolProperty;
	QString stringProperty;
	double doubleProperty;

	NormalEnum normalEnumProperty;
	EnumFlags enumFlagsProperty;

	QList<int> simpeList;
	QList<QList<int>> leveledList;

	TestObject* childObject;
	QList<TestObject*> simpleChildren;
	QList<QList<TestObject*>> leveledChildren;

	EnumFlags getEnumFlagsProperty() const;
	void setEnumFlagsProperty(const EnumFlags &value);
};

Q_DECLARE_METATYPE(TestObject*)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestObject::EnumFlags)

#endif // TESTOBJECT_H
