#include "testgadget.h"
#include <QMetaObject>
#include <QMetaProperty>

TestGadget::TestGadget() :
	intProperty(0),
	boolProperty(false),
	stringProperty(),
	doubleProperty(0.0),
	simpeList(),
	leveledList()
{}

bool TestGadget::operator==(const TestGadget &other) const
{
	return intProperty == other.intProperty &&
			boolProperty == other.boolProperty &&
			stringProperty == other.stringProperty &&
			doubleProperty == other.doubleProperty &&
			simpeList == other.simpeList &&
			leveledList == other.leveledList;
}

bool TestGadget::operator!=(const TestGadget &other) const
{
	return intProperty != other.intProperty ||
			boolProperty != other.boolProperty ||
			stringProperty != other.stringProperty ||
			doubleProperty != other.doubleProperty ||
			simpeList != other.simpeList ||
						 leveledList != other.leveledList;
}

bool TestGadget::operator<(const TestGadget &) const
{
	return false;
}

ParentGadget::ParentGadget() :
	TestGadget(),
	childGadget(),
	simpleChildren(),
	leveledChildren()
{}

bool ParentGadget::operator==(const ParentGadget &other) const
{
	return intProperty == other.intProperty &&
			boolProperty == other.boolProperty &&
			stringProperty == other.stringProperty &&
			doubleProperty == other.doubleProperty &&
			simpeList == other.simpeList &&
			leveledList == other.leveledList &&
			childGadget == other.childGadget &&
			simpleChildren == other.simpleChildren &&
			leveledChildren == other.leveledChildren;
}

bool ParentGadget::operator!=(const ParentGadget &other) const
{
	return intProperty != other.intProperty ||
			boolProperty != other.boolProperty ||
			stringProperty != other.stringProperty ||
			doubleProperty != other.doubleProperty ||
			simpeList != other.simpeList ||
			leveledList != other.leveledList ||
			childGadget != other.childGadget ||
			simpleChildren != other.simpleChildren ||
			leveledChildren != other.leveledChildren;
}

ParentGadget ParentGadget::createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty)
{
	ParentGadget t;
	t.intProperty = intProperty;
	t.boolProperty = boolProperty;
	t.stringProperty = stringProperty;
	t.doubleProperty = doubleProperty;
	return t;
}

ParentGadget ParentGadget::createList(QList<int> simpeList, QList<QList<int> > leveledList)
{
	ParentGadget t;
	t.simpeList = simpeList;
	t.leveledList = leveledList;
	return t;
}

ParentGadget ParentGadget::createChild(TestGadget childGadget, QList<TestGadget> simpleChildren, QList<QList<TestGadget> > leveledChildren)
{
	ParentGadget t;
	t.childGadget = childGadget;
	t.simpleChildren = simpleChildren;
	t.leveledChildren = leveledChildren;
	return t;
}
