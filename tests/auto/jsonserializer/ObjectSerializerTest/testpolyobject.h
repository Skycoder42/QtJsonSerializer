#ifndef TESTPOLYOBJECT_H
#define TESTPOLYOBJECT_H

#include <QObject>

class TestPolyObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int data MEMBER data)

	Q_CLASSINFO("polymorphic", "true")

public:
	explicit TestPolyObject(int data, QObject *parent = nullptr);

	int data;
};

#endif // TESTPOLYOBJECT_H
