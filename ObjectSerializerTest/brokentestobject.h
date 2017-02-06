#ifndef BROKENTESTOBJECT_H
#define BROKENTESTOBJECT_H

#include "testobject.h"

#include <QObject>

class BrokenTestObject : public TestObject
{
	Q_OBJECT
	Q_PROPERTY(BrokenTestObject::Broken broken MEMBER broken)//unserializable type

public:
	explicit BrokenTestObject(QObject *parent = nullptr);//no Q_INVOKABLE constructor

	struct Broken {
		bool operator !=(const Broken &) {
			return false;
		}
	} broken;
};

Q_DECLARE_METATYPE(BrokenTestObject::Broken)

#endif // BROKENTESTOBJECT_H
