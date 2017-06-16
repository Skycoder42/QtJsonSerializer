#ifndef BROKENTESTGADGET_H
#define BROKENTESTGADGET_H

#include "testgadget.h"

struct BrokenTestGadget : public TestGadget
{
	Q_GADGET

	Q_PROPERTY(BrokenTestGadget::Broken broken MEMBER broken)//unserializable type

public:
	BrokenTestGadget();

	struct Broken {
		bool operator !=(const Broken &) {
			return false;
		}
	} broken;
};

Q_DECLARE_METATYPE(BrokenTestGadget)
Q_DECLARE_METATYPE(BrokenTestGadget::Broken)

#endif // BROKENTESTGADGET_H
