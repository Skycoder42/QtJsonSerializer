#ifndef TESTPOLYOBJECT_H
#define TESTPOLYOBJECT_H

#include <QObject>

class PolyBase : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("polymorphic", "true")

public:
	Q_INVOKABLE explicit PolyBase(QObject *parent = nullptr);
};

class TestPolyObject : public PolyBase
{
	Q_OBJECT

	Q_PROPERTY(int data MEMBER data)

public:
	Q_INVOKABLE explicit TestPolyObject(QObject *parent = nullptr);
	explicit TestPolyObject(int data, QObject *parent);

	int data;
};

Q_DECLARE_METATYPE(PolyBase*)
Q_DECLARE_METATYPE(TestPolyObject*)

#endif // TESTPOLYOBJECT_H
