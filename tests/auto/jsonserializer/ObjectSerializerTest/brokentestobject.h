#ifndef BROKENTESTOBJECT_H
#define BROKENTESTOBJECT_H

#include "testobject.h"

class BrokenTestObject : public TestObject
{
	Q_OBJECT

	Q_PROPERTY(BrokenTestObject::Broken broken MEMBER broken)//unserializable type

public:
	Q_INVOKABLE BrokenTestObject(QObject *parent = nullptr);

	struct Broken {
		bool operator !=(const Broken &) {
			return false;
		}
	} broken;
};

class BrokenTestObject2 : public BrokenTestObject
{
	Q_OBJECT

public:
	BrokenTestObject2(QObject *parent = nullptr);//no Q_INVOKABLE constructor
};

Q_DECLARE_METATYPE(BrokenTestObject*)
Q_DECLARE_METATYPE(BrokenTestObject2*)
Q_DECLARE_METATYPE(BrokenTestObject::Broken)

#endif // BROKENTESTOBJECT_H
