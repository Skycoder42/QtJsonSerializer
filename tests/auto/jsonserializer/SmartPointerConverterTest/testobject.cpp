#include "testobject.h"

TestClass::TestClass(int value)
	: value{value}
{}

TestObject::TestObject(int value, QObject *parent)
	: QObject{parent}
	, value{value}
{}
