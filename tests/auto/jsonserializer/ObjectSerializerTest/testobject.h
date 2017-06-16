#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QFlags>
#include <QJsonObject>

class ChildObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int data MEMBER data)

public:
	int data;

	explicit ChildObject(int data, QObject *parent);
	Q_INVOKABLE ChildObject(QObject *parent);

	bool equals(const ChildObject *other) const;

	static QJsonObject createJson(const int &data = 0);
};

class TestObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int intProperty MEMBER intProperty)
	Q_PROPERTY(bool boolProperty MEMBER boolProperty)
	Q_PROPERTY(QString stringProperty MEMBER stringProperty)
	Q_PROPERTY(double doubleProperty MEMBER doubleProperty)

	Q_PROPERTY(NormalEnum normalEnumProperty MEMBER normalEnumProperty)
	Q_PROPERTY(EnumFlags enumFlagsProperty READ getEnumFlagsProperty WRITE setEnumFlagsProperty)

	Q_PROPERTY(QList<int> simpleList MEMBER simpleList)
	Q_PROPERTY(QList<QList<int>> leveledList MEMBER leveledList)

	Q_PROPERTY(QMap<QString, int> simpleMap MEMBER simpleMap)
	Q_PROPERTY(QMap<QString, QMap<QString, int>> leveledMap MEMBER leveledMap)

	Q_PROPERTY(ChildObject* childObject MEMBER childObject)

	Q_PROPERTY(QList<ChildObject*> simpleChildren MEMBER simpleChildren)
	Q_PROPERTY(QList<QList<ChildObject*>> leveledChildren MEMBER leveledChildren)

	Q_PROPERTY(QMap<QString, ChildObject*> simpleRelatives MEMBER simpleRelatives)
	Q_PROPERTY(QMap<QString, QMap<QString, ChildObject*>> leveledRelatives MEMBER leveledRelatives)

public:
	enum NormalEnum {
		Normal0 = 0,
		Normal1 = 1,
		Normal2 = 2
	};
	Q_ENUM(NormalEnum)
	enum EnumFlag {
		Flag1 = 0x02,
		Flag2 = 0x04,
		Flag3 = 0x08,

		FlagX = Flag1 | Flag2
	};
	Q_DECLARE_FLAGS(EnumFlags, EnumFlag)
	Q_FLAG(EnumFlags)

	Q_INVOKABLE TestObject(QObject *parent);

	static TestObject *createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty, QObject *parent);
	static TestObject *createEnum(NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty, QObject *parent);
	static TestObject *createList(QList<int> simpleList, QList<QList<int>> leveledList, QObject *parent);
	static TestObject *createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int>> leveledMap, QObject *parent);
	static TestObject *createChild(ChildObject* childObject, QObject *parent);
	static TestObject *createChildren(QList<ChildObject*> simpleChildren, QList<QList<ChildObject*>> leveledChildren, QObject *parent);
	static TestObject *createRelatives(QMap<QString, ChildObject*> simpleRelatives, QMap<QString, QMap<QString, ChildObject*>> leveledRelatives, QObject *parent);

	static QJsonObject createJson(const QJsonObject &delta = QJsonObject(), const QString &rmKey = {});

	static bool equals(const TestObject *left, const TestObject *right);
	bool equals(const TestObject *other) const;

	int intProperty;
	bool boolProperty;
	QString stringProperty;
	double doubleProperty;

	NormalEnum normalEnumProperty;
	EnumFlags enumFlagsProperty;

	QList<int> simpleList;
	QList<QList<int>> leveledList;

	QMap<QString, int> simpleMap;
	QMap<QString, QMap<QString, int>> leveledMap;

	ChildObject* childObject;

	QList<ChildObject*> simpleChildren;
	QList<QList<ChildObject*>> leveledChildren;

	QMap<QString, ChildObject*> simpleRelatives;
	QMap<QString, QMap<QString, ChildObject*>> leveledRelatives;

	EnumFlags getEnumFlagsProperty() const;
	void setEnumFlagsProperty(const EnumFlags &value);
};

Q_DECLARE_METATYPE(TestObject*)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestObject::EnumFlags)

#endif // TESTOBJECT_H
