#include "testgadget.h"

bool TestGadget::operator==(const TestGadget &other) const
{
	// exclude unstored properties
	return key == other.key &&
			qFuzzyCompare(value, other.value);
}
