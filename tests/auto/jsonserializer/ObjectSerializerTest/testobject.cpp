#include "testobject.h"
#include <QJsonArray>
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
	simpleMap(),
	leveledMap(),
	childObject(nullptr),
	simpleChildren(),
	leveledChildren(),
	simpleRelatives(),
	leveledRelatives(),
	object(),
	array(),
	value(QJsonValue::Null)
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

TestObject *TestObject::createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int> > leveledMap, QObject *parent)
{
	auto t = new TestObject(parent);
	t->simpleMap = simpleMap;
	t->leveledMap = leveledMap;
	return t;
}

TestObject *TestObject::createChild(ChildObject *childObject, QObject *parent)
{
	auto t = new TestObject(parent);

	t->childObject = childObject;
	if(t->childObject)
		t->childObject->setParent(t);

	return t;
}

TestObject *TestObject::createChildren(QList<ChildObject *> simpleChildren, QList<QList<ChildObject *> > leveledChildren, QObject *parent)
{
	auto t = new TestObject(parent);

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

TestObject *TestObject::createRelatives(QMap<QString, ChildObject *> simpleRelatives, QMap<QString, QMap<QString, ChildObject *> > leveledRelatives, QObject *parent)
{
	auto t = new TestObject(parent);

	t->simpleRelatives = simpleRelatives;
	foreach (auto child, t->simpleRelatives)
		child->setParent(t);

	t->leveledRelatives = leveledRelatives;
	foreach (auto children, t->leveledRelatives) {
		foreach (auto child, children)
			child->setParent(t);
	}

	return t;
}

TestObject *TestObject::createEmbedded(QJsonObject object, QJsonArray array, QJsonValue value, QObject *parent)
{
	auto t = new TestObject(parent);
	t->object = object;
	t->array = array;
	t->value = value;
	return t;
}

QJsonObject TestObject::createJson(const QJsonObject &delta, const QString &rmKey)
{
	auto base = QJsonObject({
								{"intProperty", 0},
								{"boolProperty", false},
								{"stringProperty", QString()},
								{"doubleProperty", 0},
								{"normalEnumProperty", TestObject::Normal0},
								{"enumFlagsProperty", 0},
								{"simpleList", QJsonArray()},
								{"leveledList", QJsonArray()},
								{"simpleMap", QJsonObject()},
								{"leveledMap", QJsonObject()},
								{"childObject", QJsonValue::Null},
								{"simpleChildren", QJsonArray()},
								{"leveledChildren", QJsonArray()},
								{"simpleRelatives", QJsonObject()},
								{"leveledRelatives", QJsonObject()},
								{"object", QJsonObject()},
								{"array", QJsonArray()},
								{"value", QJsonValue::Null}
							});
	for(auto it = delta.constBegin(); it != delta.constEnd(); ++it)
		base[it.key()] = it.value();
	base.remove(rmKey);
	return base;
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
				  simpleMap == other->simpleMap &&
				  leveledMap == other->leveledMap &&
				  simpleChildren.size() == other->simpleChildren.size() &&
				  leveledChildren.size() == other->leveledChildren.size() &&
				  simpleRelatives.size() == other->simpleRelatives.size() &&
				  leveledRelatives.size() == other->leveledRelatives.size() &&
				  object == other->object &&
				  array == other->array &&
				  value == other->value;
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

		for(auto it = simpleRelatives.constBegin(); it != simpleRelatives.constEnd(); ++it) {
			if(!it.value()->equals(other->simpleRelatives[it.key()]))
				return false;
		}

		for(auto it = leveledRelatives.constBegin(); it != leveledRelatives.constEnd(); ++it) {
			const auto &otherMap = other->leveledRelatives[it.key()];
			if(it->size() != otherMap.size())
				return false;
			for(auto jt = it->constBegin(); jt != it->constEnd(); ++jt) {
				if(!jt.value()->equals(otherMap[jt.key()]))
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

ChildObject::ChildObject(int data, QObject *parent) :
	QObject(parent),
	data(data),
	child(nullptr)
{}

ChildObject::ChildObject(QObject *parent) :
	QObject(parent),
	data(0),
	child(nullptr)
{}

bool ChildObject::equals(const ChildObject *other) const
{
	if(this == other)
		return true;
	else if(!other || !this)
		return false;
	else if(metaObject()->className() != other->metaObject()->className())
		return false;
	if(child && !child->equals(other->child))
		return false;
	else
		return data == other->data;
}

QJsonObject ChildObject::createJson(const int &data)
{
	return QJsonObject({
						   {"data", data},
						   {"child", QJsonValue::Null}
					   });
}
