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
	datetime(),
	uuid(),
	url(),
	version(),
	bytearray(),
	size(),
	point(),
	line(),
	rect(),
	locale(QLocale::c()),
	regexp(),
	simpleList(),
	leveledList(),
	simpleMap(),
	leveledMap(),
	pair(),
	extraPair(),
	listPair(),
	childObject(nullptr),
	sharedChildObject(),
	trackedChildObject(),
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

TestObject *TestObject::createExtra(QDateTime datetime, QUuid uuid, QUrl url, QVersionNumber version, QByteArray bytearray, QObject *parent)
{
	auto t = new TestObject(parent);
	t->datetime = datetime;
	t->uuid = uuid;
	t->url = url;
	t->version = version;
	t->bytearray = bytearray;
	return t;
}

TestObject *TestObject::createGeom(QSize size, QPoint point, QLine line, QRect rect, QObject *parent)
{
	auto t = new TestObject(parent);
	t->size = size;
	t->point = point;
	t->line = line;
	t->rect = rect;
	return t;
}

TestObject *TestObject::createSpecial(QLocale locale, QRegularExpression regexp, QObject *parent)
{
	auto t = new TestObject(parent);
	t->locale = locale;
	t->regexp = regexp;
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

TestObject *TestObject::createPair(QPair<int, QString> pair, QPair<ChildObject *, QList<int> > extraPair, QList<QPair<bool, bool> > listPair, QObject *parent)
{
	auto t = new TestObject(parent);
	t->pair = pair;
	t->extraPair = extraPair;
	if(t->extraPair.first)
		t->extraPair.first->setParent(t);
	t->listPair = listPair;
	return t;
}

TestObject *TestObject::createChild(ChildObject *childObject, QObject *parent, int memberFlag)
{
	auto t = new TestObject(parent);

	if(memberFlag == 0) {
		t->childObject = childObject;
		if(t->childObject)
			t->childObject->setParent(t);
	} else if(memberFlag == 1)
		t->sharedChildObject = QSharedPointer<ChildObject>(childObject);
	else if(memberFlag == 2) {
		t->trackedChildObject = childObject;
		if(t->trackedChildObject)
			t->trackedChildObject->setParent(t);
	} else
		Q_UNREACHABLE();

	return t;
}

TestObject *TestObject::createChildren(QList<ChildObject *> simpleChildren, QList<QList<ChildObject *> > leveledChildren, QObject *parent)
{
	auto t = new TestObject(parent);

	t->simpleChildren = simpleChildren;
	for (auto child : t->simpleChildren)
		child->setParent(t);

	t->leveledChildren = leveledChildren;
	for (auto children : t->leveledChildren) {
		for (auto child : children)
			child->setParent(t);
	}

	return t;
}

TestObject *TestObject::createRelatives(QMap<QString, ChildObject *> simpleRelatives, QMap<QString, QMap<QString, ChildObject *> > leveledRelatives, QObject *parent)
{
	auto t = new TestObject(parent);

	t->simpleRelatives = simpleRelatives;
	for (auto child : t->simpleRelatives)
		child->setParent(t);

	t->leveledRelatives = leveledRelatives;
	for (auto children : t->leveledRelatives) {
		for (auto child : children)
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
								{"datetime", QString()},
								{"uuid", QStringLiteral("{00000000-0000-0000-0000-000000000000}")},
								{"url", QString()},
								{"version", QString()},
								{"bytearray", QString()},
								{"size", QJsonObject({
									 {"width", -1},
									 {"height", -1}
								 })},
								{"point", QJsonObject({
									 {"x", 0},
									 {"y", 0}
								 })},
								{"line", QJsonObject({
									 {"p1", QJsonObject({
										  {"x", 0},
										  {"y", 0}
									  })},
									 {"p2", QJsonObject({
										  {"x", 0},
										  {"y", 0}
									  })}
								 })},
								{"rect", QJsonObject({
									 {"topLeft", QJsonObject({
										  {"x", 0},
										  {"y", 0}
									  })},
									 {"bottomRight", QJsonObject({
										  {"x", -1},
										  {"y", -1}
									  })}
								 })},
								{"locale", QLocale::c().bcp47Name()},
								{"regexp", QJsonObject({
									 {"pattern", QString()},
									 {"options", 0}
								 })},
								{"simpleList", QJsonArray()},
								{"leveledList", QJsonArray()},
								{"simpleMap", QJsonObject()},
								{"leveledMap", QJsonObject()},
								{"pair", QJsonArray({0, QString()})},
								{"extraPair", QJsonArray({QJsonValue::Null, QJsonArray()})},
								{"listPair", QJsonArray()},
								{"childObject", QJsonValue::Null},
								{"sharedChildObject", QJsonValue::Null},
								{"trackedChildObject", QJsonValue::Null},
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
				  datetime == other->datetime &&
				  uuid == other->uuid &&
				  url == other->url &&
				  version == other->version &&
				  bytearray == other->bytearray &&
				  size == other->size &&
				  point == other->point &&
				  line == other->line &&
				  rect == other->rect &&
				  locale.bcp47Name() == other->locale.bcp47Name() &&
				  regexp == other->regexp &&
				  simpleList == other->simpleList &&
				  leveledList == other->leveledList &&
				  simpleMap == other->simpleMap &&
				  leveledMap == other->leveledMap &&
				  pair == other->pair &&
				  extraPair.second == other->extraPair.second &&
				  listPair == other->listPair &&
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
		if(!sharedChildObject.data()->equals(other->sharedChildObject.data()))
			return false;
		if(!trackedChildObject.data()->equals(other->trackedChildObject.data()))
			return false;
		if(!extraPair.first->equals(other->extraPair.first))
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
