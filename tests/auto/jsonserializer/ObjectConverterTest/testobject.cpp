#include "testobject.h"

TestObject::TestObject(QObject *parent) :
	QObject{parent}
{}

TestObject::TestObject(int key, double value, int zhidden, QObject *parent) :
	QObject{parent},
	key{key},
	value{value},
	zhidden{zhidden}
{
	setObjectName(QStringLiteral("TestObject"));
}

bool TestObject::compare(const TestObject *other) const
{
	return key == other->key &&
		   qFuzzyCompare(value, other->value) &&
		   zhidden == other->zhidden;
}

StaticPolyObject::StaticPolyObject(QObject *parent) :
	TestObject{parent}
{}

StaticPolyObject::StaticPolyObject(int key, double value, int hidden, bool extra1, QObject *parent) :
	TestObject{key, value, hidden, parent},
	extra1{extra1}
{}

bool StaticPolyObject::compare(const TestObject *other) const
{
	auto o = qobject_cast<const StaticPolyObject*>(other);
	if(o) {
		return TestObject::compare(o) &&
				extra1 == o->extra1;
	} else
		return false;
}

StaticNonPolyObject::StaticNonPolyObject(QObject *parent) :
	TestObject{parent}
{}

StaticNonPolyObject::StaticNonPolyObject(int key, double value, int hidden, bool extra2, QObject *parent) :
	TestObject{key, value, hidden, parent},
	extra2{extra2}
{}

bool StaticNonPolyObject::compare(const TestObject *other) const
{
	auto o = qobject_cast<const StaticNonPolyObject*>(other);
	if(o) {
		return TestObject::compare(o) &&
				extra2 == o->extra2;
	} else
		return false;
}

DynamicPolyObject::DynamicPolyObject(QObject *parent) :
	TestObject{parent}
{
	setProperty("__qt_json_serializer_polymorphic", false);
}

DynamicPolyObject::DynamicPolyObject(int key, double value, int hidden, bool extra3, bool poly, QObject *parent) :
	TestObject{key, value, hidden, parent},
	extra3{extra3}
{
	setProperty("__qt_json_serializer_polymorphic", poly);
}

bool DynamicPolyObject::compare(const TestObject *other) const
{
	auto o = qobject_cast<const DynamicPolyObject*>(other);
	if(o) {
		return TestObject::compare(o) &&
				extra3 == o->extra3;
	} else
		return false;
}

BrokenObject::BrokenObject(QObject *parent) :
	TestObject{parent}
{}

DerivedTestObject::DerivedTestObject(QObject *parent) :
	TestObject{parent}
{}

DerivedTestObject::DerivedTestObject(int key, double value, int hidden, bool extra4, QObject *parent) :
	TestObject{key, value, hidden, parent},
	extra4{extra4}
{}

bool DerivedTestObject::compare(const TestObject *other) const
{
	auto o = qobject_cast<const DerivedTestObject*>(other);
	if(o) {
		return TestObject::compare(o) &&
				extra4 == o->extra4;
	} else
		return false;
}
