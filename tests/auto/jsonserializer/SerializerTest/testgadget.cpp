#include "testgadget.h"

TestGadget::TestGadget(int data) :
	data{data}
{}

bool TestGadget::operator==(TestGadget other) const
{
	return data == other.data;
}

EnumGadget::EnumGadget() = default;

EnumGadget::EnumGadget(EnumGadget::NormalEnum enumProp) :
	enumProp{enumProp}
{}

EnumGadget::EnumGadget(EnumFlag flagsProp) :
	flagsProp{flagsProp}
{}

EnumGadget::EnumGadget(EnumFlags flagsProp) :
	flagsProp{flagsProp}
{}

bool EnumGadget::operator==(EnumGadget other) const
{
	return enumProp == other.enumProp &&
			flagsProp == other.flagsProp;
}

EnumGadget::EnumFlags EnumGadget::getFlagsProp() const
{
	return flagsProp;
}

void EnumGadget::setFlagsProp(EnumFlags value)
{
	flagsProp = value;
}
