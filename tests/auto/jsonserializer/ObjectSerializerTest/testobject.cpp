#include "testobject.h"
#include <QMetaObject>
#include <QMetaProperty>

TestObject::TestObject(QObject *parent) :
	QObject(parent),
	intProperty(0),
	boolProperty(false),
	stringProperty(),
	doubleProperty(0.0),
	normalEnumProperty(Normal0),
	enumFlagsProperty(0x00),
	simpleList(),
	leveledList(),
	childObject(nullptr),
	simpleChildren(),
	leveledChildren()
{}

TestObject *TestObject::createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty, QObject *parent)
{
	auto t = new TestObject(parent);
	t->intProperty = intProperty;
	t->boolProperty = boolProperty;
	t->stringProperty = stringProperty;
	t->doubleProperty = doubleProperty;
	return t;
}

TestObject *TestObject::createEnum(TestObject::NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty, QObject *parent)
{
	auto t = new TestObject(parent);
	t->normalEnumProperty = normalEnumProperty;
	t->enumFlagsProperty = enumFlagsProperty;
	return t;
}

TestObject *TestObject::createList(QList<int> simpleList, QList<QList<int> > leveledList, QObject *parent)
{
	auto t = new TestObject(parent);
	t->simpleList = simpleList;
	t->leveledList = leveledList;
	return t;
}

TestObject *TestObject::createChild(TestObject *childObject, QList<TestObject *> simpleChildren, QList<QList<TestObject *> > leveledChildren, QObject *parent)
{
	auto t = new TestObject(parent);

	t->childObject = childObject;
	if(t->childObject)
		t->childObject->setParent(t);

	t->simpleChildren = simpleChildren;
	foreach (auto child, t->simpleChildren)
		child->setParent(t);

	t->leveledChildren = leveledChildren;
	foreach (auto children, t->leveledChildren) {
		foreach (auto child, children)
			child->setParent(t);
	}

	return t;
}

bool TestObject::equals(const TestObject *left, const TestObject *right)
{
	if(left) {
		if(!left->equals(right))
			return false;
	} else if(left != right)
		return false;
	return true;
}

bool TestObject::equals(const TestObject *other) const
{
	if(this == other)
		return true;
	else if(!other || !this)
		return false;
	else if(metaObject()->className() != other->metaObject()->className())
		return false;
	else {
		auto ok = intProperty == other->intProperty &&
				  boolProperty == other->boolProperty &&
				  stringProperty == other->stringProperty &&
				  doubleProperty == other->doubleProperty &&
				  normalEnumProperty == other->normalEnumProperty &&
				  enumFlagsProperty == other->enumFlagsProperty &&
				  simpleList == other->simpleList &&
				  leveledList == other->leveledList &&
				  simpleChildren.size() == other->simpleChildren.size() &&
				  leveledChildren.size() == other->leveledChildren.size() ;
		if(!ok)
			return false;
		if(!childObject->equals(other->childObject))
			return false;
		for(auto i = 0; i < simpleChildren.size(); i++) {
			if(!simpleChildren[i]->equals(other->simpleChildren[i]))
				return false;
		}
		for(auto i = 0; i < leveledChildren.size(); i++) {
			if(leveledChildren[i].size() != other->leveledChildren[i].size())
				return false;
			for(auto j = 0; j < leveledChildren[i].size(); j++) {
				if(!leveledChildren[i][j]->equals(other->leveledChildren[i][j]))
					return false;
			}
		}

		return true;
	}
}

TestObject::EnumFlags TestObject::getEnumFlagsProperty() const
{
	return enumFlagsProperty;
}

void TestObject::setEnumFlagsProperty(const EnumFlags &value)
{
	enumFlagsProperty = value;
}
