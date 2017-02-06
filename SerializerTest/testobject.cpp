#include "testobject.h"
#include <QMetaObject>
#include <QMetaProperty>

TestObject::TestObject(QObject *parent) :
	QObject(parent),
	intProperty(0),
	boolProperty(false),
	stringProperty(),
	doubleProperty(0.0),
	simpeList(),
	leveledList(),
	childObject(nullptr),
	simpleChildren(),
	leveledChildren()
{}

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
	else if(!other)
		return false;
	else if(metaObject()->className() != other->metaObject()->className())
		return false;
	else {
		auto ok = intProperty == other->intProperty &&
				  boolProperty == other->boolProperty &&
				  stringProperty == other->stringProperty &&
				  doubleProperty == other->doubleProperty &&
				  simpeList == other->simpeList &&
				  leveledList == other->leveledList &&
				  simpleChildren.size() == other->simpleChildren.size() &&
				  leveledChildren.size() == other->leveledChildren.size() ;
		if(!ok)
			return false;
		if(!childObject->equals(other))
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
