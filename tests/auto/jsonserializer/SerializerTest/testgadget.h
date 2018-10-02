#ifndef TESTGADGET_H
#define TESTGADGET_H

#include <QObject>

struct TestGadget
{
	Q_GADGET

	Q_PROPERTY(int data MEMBER data)

public:
	TestGadget(int data = 0);

	int data;

	bool operator==(TestGadget other) const;
};

class CustomGadget
{
	Q_GADGET

	Q_PROPERTY(int data MEMBER data)

public:
	CustomGadget(int data = 0);

	int data;

	bool operator==(CustomGadget other) const;
	bool operator!=(CustomGadget other) const;
};

using IntAlias = int;
using ListAlias = QList<CustomGadget>;

struct AliasGadget
{
	Q_GADGET

	Q_PROPERTY(IntAlias intAlias MEMBER intAlias)
	Q_PROPERTY(ListAlias listAlias MEMBER listAlias)
	Q_PROPERTY(QList<CustomGadget> classList MEMBER classList)

public:
	AliasGadget(int data1 = 0, int data2 = 0, int data3 = 0);

	IntAlias intAlias;
	ListAlias listAlias;
	QList<CustomGadget> classList;

	bool operator==(const AliasGadget &other) const;
};

struct EnumGadget
{
	Q_GADGET

	Q_PROPERTY(NormalEnum enumProp MEMBER enumProp)
	Q_PROPERTY(EnumFlags flagsProp READ getFlagsProp WRITE setFlagsProp)

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

	EnumGadget();
	EnumGadget(NormalEnum enumProp);
	EnumGadget(EnumFlag flagsProp);
	EnumGadget(EnumFlags flagsProp);

	NormalEnum enumProp = Normal0;
	EnumFlags flagsProp;

	bool operator==(EnumGadget other) const;

	EnumFlags getFlagsProp() const;
	void setFlagsProp(EnumFlags value);
};

Q_DECLARE_METATYPE(TestGadget)
Q_DECLARE_TYPEINFO(TestGadget, Q_PRIMITIVE_TYPE);

Q_DECLARE_METATYPE(CustomGadget)
Q_DECLARE_TYPEINFO(CustomGadget, Q_PRIMITIVE_TYPE);
Q_DECLARE_METATYPE(ListAlias)
Q_DECLARE_METATYPE(AliasGadget)
Q_DECLARE_TYPEINFO(AliasGadget, Q_PRIMITIVE_TYPE);

Q_DECLARE_METATYPE(EnumGadget)
Q_DECLARE_TYPEINFO(EnumGadget, Q_PRIMITIVE_TYPE);
Q_DECLARE_OPERATORS_FOR_FLAGS(EnumGadget::EnumFlags)

#endif // TESTGADGET_H
