#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <QList>

class TestObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int intProperty MEMBER intProperty)
	Q_PROPERTY(bool boolProperty MEMBER boolProperty)
	Q_PROPERTY(QString stringProperty MEMBER stringProperty)
	Q_PROPERTY(double doubleProperty MEMBER doubleProperty)

	Q_PROPERTY(QList<int> simpeList MEMBER simpeList)
	Q_PROPERTY(QList<QList<int>> leveledList MEMBER leveledList)

	Q_PROPERTY(TestObject* childObject MEMBER childObject)
	Q_PROPERTY(QList<TestObject*> simpleChildren MEMBER simpleChildren)
	Q_PROPERTY(QList<QList<TestObject*>> leveledChildren MEMBER leveledChildren)

public:
	explicit TestObject(QObject *parent = nullptr);

	static bool equals(const TestObject *left, const TestObject *right);
	bool equals(const TestObject *other) const;

	int intProperty;
	bool boolProperty;
	QString stringProperty;
	double doubleProperty;

	QList<int> simpeList;
	QList<QList<int>> leveledList;

	TestObject* childObject;
	QList<TestObject*> simpleChildren;
	QList<QList<TestObject*>> leveledChildren;
};

#endif // TESTOBJECT_H
