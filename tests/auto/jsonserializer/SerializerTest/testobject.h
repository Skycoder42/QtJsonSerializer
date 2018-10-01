#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>

class TestObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int data MEMBER data)

public:
	int data = 0;

	explicit TestObject(int data, QObject *parent = nullptr);
	Q_INVOKABLE TestObject(QObject *parent);

	static bool equals(const TestObject *lhs, const TestObject *rhs);
};

Q_DECLARE_METATYPE(TestObject*)

#endif // TESTOBJECT_H
