#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QDateTime>
#include <QUuid>
#include <QUrl>
#include <QFlags>
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedPointer>
#include <QPointer>

class ChildObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int data MEMBER data)
	Q_PROPERTY(ChildObject *child MEMBER child)

public:
	int data;
	ChildObject *child;

	explicit ChildObject(int data, QObject *parent = nullptr);
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

	Q_PROPERTY(QDateTime datetime MEMBER datetime)
	Q_PROPERTY(QUuid uuid MEMBER uuid)
	Q_PROPERTY(QUrl url MEMBER url)

	Q_PROPERTY(QList<int> simpleList MEMBER simpleList)
	Q_PROPERTY(QList<QList<int>> leveledList MEMBER leveledList)

	Q_PROPERTY(QMap<QString, int> simpleMap MEMBER simpleMap)
	Q_PROPERTY(QMap<QString, QMap<QString, int>> leveledMap MEMBER leveledMap)

	Q_PROPERTY(QPair<int, QString> pair MEMBER pair)
	Q_PROPERTY(QPair<ChildObject*, QList<int>> extraPair MEMBER extraPair)
	Q_PROPERTY(QList<QPair<bool, bool>> listPair MEMBER listPair)

	Q_PROPERTY(ChildObject* childObject MEMBER childObject)
	Q_PROPERTY(QSharedPointer<ChildObject> sharedChildObject MEMBER sharedChildObject)
	Q_PROPERTY(QPointer<ChildObject> trackedChildObject MEMBER trackedChildObject)

	Q_PROPERTY(QList<ChildObject*> simpleChildren MEMBER simpleChildren)
	Q_PROPERTY(QList<QList<ChildObject*>> leveledChildren MEMBER leveledChildren)

	Q_PROPERTY(QMap<QString, ChildObject*> simpleRelatives MEMBER simpleRelatives)
	Q_PROPERTY(QMap<QString, QMap<QString, ChildObject*>> leveledRelatives MEMBER leveledRelatives)

	Q_PROPERTY(QJsonObject object MEMBER object)
	Q_PROPERTY(QJsonArray array MEMBER array)
	Q_PROPERTY(QJsonValue value MEMBER value)

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
	static TestObject *createExtra(QDateTime datetime, QUuid uuid, QUrl url, QObject *parent);
	static TestObject *createList(QList<int> simpleList, QList<QList<int>> leveledList, QObject *parent);
	static TestObject *createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int>> leveledMap, QObject *parent);
	static TestObject *createPair(QPair<int, QString> pair, QPair<ChildObject*, QList<int>> extraPair, QList<QPair<bool, bool>> listPair, QObject *parent);
	static TestObject *createChild(ChildObject* childObject, QObject *parent, int memberFlag = 0);//0: ptr, 1: sp, 2: tp
	static TestObject *createChildren(QList<ChildObject*> simpleChildren, QList<QList<ChildObject*>> leveledChildren, QObject *parent);
	static TestObject *createRelatives(QMap<QString, ChildObject*> simpleRelatives, QMap<QString, QMap<QString, ChildObject*>> leveledRelatives, QObject *parent);
	static TestObject *createEmbedded(QJsonObject object, QJsonArray array, QJsonValue value, QObject *parent);

	static QJsonObject createJson(const QJsonObject &delta = QJsonObject(), const QString &rmKey = {});

	static bool equals(const TestObject *left, const TestObject *right);
	bool equals(const TestObject *other) const;

	int intProperty;
	bool boolProperty;
	QString stringProperty;
	double doubleProperty;

	NormalEnum normalEnumProperty;
	EnumFlags enumFlagsProperty;

	QDateTime datetime;
	QUuid uuid;
	QUrl url;

	QList<int> simpleList;
	QList<QList<int>> leveledList;

	QMap<QString, int> simpleMap;
	QMap<QString, QMap<QString, int>> leveledMap;

	QPair<int, QString> pair;
	QPair<ChildObject*, QList<int>> extraPair;
	QList<QPair<bool, bool>> listPair;

	ChildObject* childObject;
	QSharedPointer<ChildObject> sharedChildObject;
	QPointer<ChildObject> trackedChildObject;

	QList<ChildObject*> simpleChildren;
	QList<QList<ChildObject*>> leveledChildren;

	QMap<QString, ChildObject*> simpleRelatives;
	QMap<QString, QMap<QString, ChildObject*>> leveledRelatives;

	QJsonObject object;
	QJsonArray array;
	QJsonValue value;

	EnumFlags getEnumFlagsProperty() const;
	void setEnumFlagsProperty(const EnumFlags &value);
};

Q_DECLARE_METATYPE(TestObject*)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestObject::EnumFlags)

#endif // TESTOBJECT_H
