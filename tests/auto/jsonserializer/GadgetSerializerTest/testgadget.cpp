#include "testgadget.h"
#include <QMetaObject>
#include <QMetaProperty>

TestGadget::TestGadget() :
	intProperty(0),
	boolProperty(false),
	stringProperty(),
	doubleProperty(0.0),
	normalEnumProperty(Normal0),
	enumFlagsProperty(0),
	simpleList(),
	leveledList()
{}

bool TestGadget::operator==(const TestGadget &other) const
{
	return intProperty == other.intProperty &&
			boolProperty == other.boolProperty &&
			stringProperty == other.stringProperty &&
			doubleProperty == other.doubleProperty &&
			normalEnumProperty == other.normalEnumProperty &&
			enumFlagsProperty == other.enumFlagsProperty &&
			simpleList == other.simpleList &&
			leveledList == other.leveledList;
}

bool TestGadget::operator!=(const TestGadget &other) const
{
	return intProperty != other.intProperty ||
			boolProperty != other.boolProperty ||
			stringProperty != other.stringProperty ||
			doubleProperty != other.doubleProperty ||
			normalEnumProperty != other.normalEnumProperty ||
			enumFlagsProperty != other.enumFlagsProperty ||
			simpleList != other.simpleList ||
						 leveledList != other.leveledList;
}

bool TestGadget::operator<(const TestGadget &) const
{
	return false;
}

TestGadget::EnumFlags TestGadget::getEnumFlagsProperty() const
{
	return enumFlagsProperty;
}

void TestGadget::setEnumFlagsProperty(const EnumFlags &value)
{
	enumFlagsProperty = value;
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
			simpleList == other.simpleList &&
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
			simpleList != other.simpleList ||
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

ParentGadget ParentGadget::createEnum(TestGadget::NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty)
{
	ParentGadget t;
	t.normalEnumProperty = normalEnumProperty;
	t.enumFlagsProperty = enumFlagsProperty;
	return t;
}

ParentGadget ParentGadget::createList(QList<int> simpleList, QList<QList<int> > leveledList)
{
	ParentGadget t;
	t.simpleList = simpleList;
	t.leveledList = leveledList;
	return t;
}

ParentGadget ParentGadget::createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int> > leveledMap)
{
	ParentGadget t;
	t.simpleMap = simpleMap;
	t.leveledMap = leveledMap;
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
