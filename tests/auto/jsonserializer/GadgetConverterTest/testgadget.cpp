#include "testgadget.h"

TestGadget::TestGadget(int key, double value, int zhidden) :
	key(key),
	value(value),
	zhidden(zhidden)
{}

bool TestGadget::operator==(const TestGadget &other) const
{
	// exclude unstored properties
	return key == other.key &&
			qFuzzyCompare(value, other.value);
}
