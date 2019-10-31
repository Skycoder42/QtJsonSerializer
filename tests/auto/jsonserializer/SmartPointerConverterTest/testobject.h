#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <QSharedPointer>

class TestClass
{
public:
	int value;

	TestClass(int value = 0);
};

class TestObject : public QObject
{
	Q_OBJECT

public:
	int value;

	explicit TestObject(int value = 0, QObject *parent = nullptr);
};

Q_DECLARE_METATYPE(TestClass*)
Q_DECLARE_METATYPE(QSharedPointer<TestClass>)
Q_DECLARE_METATYPE(TestObject*)

#endif // TESTOBJECT_H
