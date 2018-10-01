#ifndef OPAQUEDUMMY_H
#define OPAQUEDUMMY_H

#include <QtCore/QMetaType>

class OpaqueDummy {};

class OpaqueDummyGadget
{
	Q_GADGET
};

Q_DECLARE_METATYPE(OpaqueDummy)
Q_DECLARE_METATYPE(OpaqueDummy*)
Q_DECLARE_METATYPE(OpaqueDummyGadget)
Q_DECLARE_METATYPE(OpaqueDummyGadget*)

#endif // OPAQUEDUMMY_H
