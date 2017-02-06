#include "brokentestobject.h"

BrokenTestObject::BrokenTestObject(QObject *parent) :
	TestObject(parent)
{}

BrokenTestObject2::BrokenTestObject2(QObject *parent) :
	BrokenTestObject(parent)
{}
