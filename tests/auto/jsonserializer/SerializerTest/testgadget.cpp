#include "testgadget.h"

TestGadget::TestGadget(int data) :
	data{data}
{}

bool TestGadget::operator==(TestGadget other) const
{
	return data == other.data;
}

bool EnumGadget::operator==(EnumGadget other) const
{
	return normalEnumProperty == other.normalEnumProperty &&
			enumFlagsProperty == other.enumFlagsProperty;
}

EnumGadget::EnumFlags EnumGadget::getEnumFlagsProperty() const
{
	return enumFlagsProperty;
}

void EnumGadget::setEnumFlagsProperty(EnumFlags value)
{
	enumFlagsProperty = value;
}
