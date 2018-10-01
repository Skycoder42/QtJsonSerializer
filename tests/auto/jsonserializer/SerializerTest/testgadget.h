#ifndef TESTGADGET_H
#define TESTGADGET_H

#include <QObject>

struct TestGadget
{
	Q_GADGET

	Q_PROPERTY(int data MEMBER data)

public:
	int data;

	TestGadget(int data = 0);

	bool operator==(TestGadget other) const;
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

	bool operator==(EnumGadget other) const;

	NormalEnum enumProp = Normal0;
	EnumFlags flagsProp;

	EnumFlags getFlagsProp() const;
	void setFlagsProp(EnumFlags value);
};

Q_DECLARE_METATYPE(TestGadget)
Q_DECLARE_METATYPE(EnumGadget)
Q_DECLARE_OPERATORS_FOR_FLAGS(EnumGadget::EnumFlags)

Q_DECLARE_TYPEINFO(TestGadget, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(EnumGadget, Q_PRIMITIVE_TYPE);

#endif // TESTGADGET_H
