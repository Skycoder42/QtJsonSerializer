#include "testobject.h"

TestObject::TestObject(int data, QObject *parent) :
	QObject{parent},
	data{data}
{
	setObjectName(QStringLiteral("testname"));
}

TestObject::TestObject(QObject *parent) :
	QObject{parent}
{}

bool TestObject::equals(const TestObject *lhs, const TestObject *rhs)
{
	if(lhs == rhs)
		return true;
	else if(!lhs || !rhs)
		return false;
	else
		return lhs->data == rhs->data;
}
