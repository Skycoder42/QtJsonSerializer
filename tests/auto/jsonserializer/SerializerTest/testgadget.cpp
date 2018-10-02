#include "testgadget.h"

TestGadget::TestGadget(int data) :
	data{data}
{}

bool TestGadget::operator==(TestGadget other) const
{
	return data == other.data;
}



CustomGadget::CustomGadget(int data) :
	data{data}
{}

bool CustomGadget::operator==(CustomGadget other) const
{
	return data == other.data;
}

bool CustomGadget::operator!=(CustomGadget other) const
{
	return data != other.data;
}

AliasGadget::AliasGadget(int data1, int data2, int data3) :
	intAlias{data1},
	listAlias{{data2}},
	classList{{data3}}
{}

bool AliasGadget::operator==(const AliasGadget &other) const
{
	return intAlias == other.intAlias &&
			listAlias == other.listAlias &&
			classList == other.classList;
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
