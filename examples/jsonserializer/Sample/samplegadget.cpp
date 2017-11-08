#include "samplegadget.h"

SampleGadget::SampleGadget() :
	base(),
	rawData()
{}

bool SampleGadget::operator !=(const SampleGadget &other) const
{
	return base != other.base ||
		rawData != other.rawData;
}
