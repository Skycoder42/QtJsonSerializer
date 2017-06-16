#ifndef TESTGADGET_H
#define TESTGADGET_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QJsonObject>

struct ChildGadget
{
	Q_GADGET

	Q_PROPERTY(int data MEMBER data)

public:
	int data;

	ChildGadget(int data = 0);

	bool operator==(const ChildGadget &other) const;
	bool operator!=(const ChildGadget &other) const;
	bool operator<(const ChildGadget &other) const;

	static QJsonObject createJson(const int &data = 0);
};

struct TestGadget
{
	Q_GADGET

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

	Q_PROPERTY(ChildGadget childGadget MEMBER childGadget)

	Q_PROPERTY(QList<ChildGadget> simpleChildren MEMBER simpleChildren)
	Q_PROPERTY(QList<QList<ChildGadget>> leveledChildren MEMBER leveledChildren)

	Q_PROPERTY(QMap<QString, ChildGadget> simpleRelatives MEMBER simpleRelatives)
	Q_PROPERTY(QMap<QString, QMap<QString, ChildGadget>> leveledRelatives MEMBER leveledRelatives)

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

	TestGadget();

	bool operator==(const TestGadget &other) const;
	bool operator!=(const TestGadget &other) const;
	bool operator<(const TestGadget &other) const;

	static TestGadget createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty);
	static TestGadget createEnum(NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty);
	static TestGadget createList(QList<int> simpleList, QList<QList<int>> leveledList);
	static TestGadget createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int>> leveledMap);
	static TestGadget createChild(ChildGadget childGadget);
	static TestGadget createChildren(QList<ChildGadget> simpleChildren, QList<QList<ChildGadget>> leveledChildren);
	static TestGadget createRelatives(QMap<QString, ChildGadget> simpleRelatives, QMap<QString, QMap<QString, ChildGadget>> leveledRelatives);

	static QJsonObject createJson(const QJsonObject &delta = QJsonObject());

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

	ChildGadget childGadget;

	QList<ChildGadget> simpleChildren;
	QList<QList<ChildGadget>> leveledChildren;

	QMap<QString, ChildGadget> simpleRelatives;
	QMap<QString, QMap<QString, ChildGadget>> leveledRelatives;

	EnumFlags getEnumFlagsProperty() const;
	void setEnumFlagsProperty(const EnumFlags &value);
};

Q_DECLARE_METATYPE(TestGadget)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestGadget::EnumFlags)

#endif // TESTGADGET_H
