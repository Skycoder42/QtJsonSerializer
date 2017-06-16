#include "testgadget.h"
#include <QJsonArray>
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
	leveledList(),
	simpleMap(),
	leveledMap(),
	childGadget(),
	simpleChildren(),
	leveledChildren(),
	simpleRelatives(),
	leveledRelatives()
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
			leveledList == other.leveledList &&
			simpleMap == other.simpleMap &&
			leveledMap == other.leveledMap &&
			childGadget == other.childGadget &&
			simpleChildren == other.simpleChildren &&
			leveledChildren == other.leveledChildren &&
			simpleRelatives == other.simpleRelatives &&
			leveledRelatives == other.leveledRelatives;
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
			leveledList != other.leveledList ||
			simpleMap != other.simpleMap ||
			leveledMap != other.leveledMap ||
			childGadget != other.childGadget ||
			simpleChildren != other.simpleChildren ||
			leveledChildren != other.leveledChildren ||
			simpleRelatives != other.simpleRelatives ||
			leveledRelatives != other.leveledRelatives;
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

TestGadget TestGadget::createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty)
{
	TestGadget t;
	t.intProperty = intProperty;
	t.boolProperty = boolProperty;
	t.stringProperty = stringProperty;
	t.doubleProperty = doubleProperty;
	return t;
}

TestGadget TestGadget::createEnum(TestGadget::NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty)
{
	TestGadget t;
	t.normalEnumProperty = normalEnumProperty;
	t.enumFlagsProperty = enumFlagsProperty;
	return t;
}

TestGadget TestGadget::createList(QList<int> simpleList, QList<QList<int> > leveledList)
{
	TestGadget t;
	t.simpleList = simpleList;
	t.leveledList = leveledList;
	return t;
}

TestGadget TestGadget::createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int> > leveledMap)
{
	TestGadget t;
	t.simpleMap = simpleMap;
	t.leveledMap = leveledMap;
	return t;
}

TestGadget TestGadget::createChild(ChildGadget childGadget)
{
	TestGadget t;
	t.childGadget = childGadget;
	return t;
}

TestGadget TestGadget::createChildren(QList<ChildGadget> simpleChildren, QList<QList<ChildGadget> > leveledChildren)
{
	TestGadget t;
	t.simpleChildren = simpleChildren;
	t.leveledChildren = leveledChildren;
	return t;
}

TestGadget TestGadget::createRelatives(QMap<QString, ChildGadget> simpleRelatives, QMap<QString, QMap<QString, ChildGadget> > leveledRelatives)
{
	TestGadget t;
	t.simpleRelatives = simpleRelatives;
	t.leveledRelatives = leveledRelatives;
	return t;
}

QJsonObject TestGadget::createJson(const QJsonObject &delta, const QString &rmKey)
{
	auto base = QJsonObject({
								{"intProperty", 0},
								{"boolProperty", false},
								{"stringProperty", QString()},
								{"doubleProperty", 0},
								{"normalEnumProperty", TestGadget::Normal0},
								{"enumFlagsProperty", 0},
								{"simpleList", QJsonArray()},
								{"leveledList", QJsonArray()},
								{"simpleMap", QJsonObject()},
								{"leveledMap", QJsonObject()},
								{"childGadget", ChildGadget::createJson()},
								{"simpleChildren", QJsonArray()},
								{"leveledChildren", QJsonArray()},
								{"simpleRelatives", QJsonObject()},
								{"leveledRelatives", QJsonObject()}
							});
	for(auto it = delta.constBegin(); it != delta.constEnd(); ++it)
		base[it.key()] = it.value();
	base.remove(rmKey);
	return base;
}

ChildGadget::ChildGadget(int data) :
	data(data)
{}

bool ChildGadget::operator==(const ChildGadget &other) const
{
	return data == other.data;
}

bool ChildGadget::operator!=(const ChildGadget &other) const
{
	return data != other.data;
}

bool ChildGadget::operator<(const ChildGadget &other) const
{
	return data < other.data;
}

QJsonObject ChildGadget::createJson(const int &data)
{
	return QJsonObject({
						   {"data", data},
					   });
}
