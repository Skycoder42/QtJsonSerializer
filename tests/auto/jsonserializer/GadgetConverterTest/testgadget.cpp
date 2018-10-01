#include "testgadget.h"

TestGadget::TestGadget(int key, double value, int hidden) :
	key(key),
	value(value),
	hidden(hidden)
{}

bool TestGadget::operator==(const TestGadget &other) const
{
	// exclude unstored properties
	return key == other.key &&
			qFuzzyCompare(value, other.value);
}
