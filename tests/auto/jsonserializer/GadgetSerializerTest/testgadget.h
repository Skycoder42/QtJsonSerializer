#ifndef TESTGADGET_H
#define TESTGADGET_H

#include <QObject>
#include <QList>

struct TestGadget
{
	Q_GADGET

	Q_PROPERTY(int intProperty MEMBER intProperty)
	Q_PROPERTY(bool boolProperty MEMBER boolProperty)
	Q_PROPERTY(QString stringProperty MEMBER stringProperty)
	Q_PROPERTY(double doubleProperty MEMBER doubleProperty)

	Q_PROPERTY(NormalEnum normalEnumProperty MEMBER normalEnumProperty)
	Q_PROPERTY(EnumFlags enumFlagsProperty READ getEnumFlagsProperty WRITE setEnumFlagsProperty)

	Q_PROPERTY(QList<int> simpeList MEMBER simpeList)
	Q_PROPERTY(QList<QList<int>> leveledList MEMBER leveledList)

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

	int intProperty;
	bool boolProperty;
	QString stringProperty;
	double doubleProperty;

	NormalEnum normalEnumProperty;
	EnumFlags enumFlagsProperty;

	QList<int> simpeList;
	QList<QList<int>> leveledList;

	EnumFlags getEnumFlagsProperty() const;
	void setEnumFlagsProperty(const EnumFlags &value);
};

Q_DECLARE_METATYPE(TestGadget)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestGadget::EnumFlags)

struct ParentGadget : public TestGadget
{
	Q_GADGET

	Q_PROPERTY(TestGadget childGadget MEMBER childGadget)
	Q_PROPERTY(QList<TestGadget> simpleChildren MEMBER simpleChildren)
	Q_PROPERTY(QList<QList<TestGadget>> leveledChildren MEMBER leveledChildren)

public:
	ParentGadget();

	bool operator==(const ParentGadget &other) const;
	bool operator!=(const ParentGadget &other) const;


	static ParentGadget createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty);
	static ParentGadget createEnum(NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty);
	static ParentGadget createList(QList<int> simpeList, QList<QList<int>> leveledList);
	static ParentGadget createChild(TestGadget childGadget, QList<TestGadget> simpleChildren, QList<QList<TestGadget>> leveledChildren);

	TestGadget childGadget;
	QList<TestGadget> simpleChildren;
	QList<QList<TestGadget>> leveledChildren;
};

Q_DECLARE_METATYPE(ParentGadget)

#endif // TESTGADGET_H
