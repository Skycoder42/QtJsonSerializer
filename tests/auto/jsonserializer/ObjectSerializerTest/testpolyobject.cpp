#include "testpolyobject.h"

PolyBase::PolyBase(QObject *parent) :
	QObject(parent)
{}

TestPolyObject::TestPolyObject(QObject *parent) :
	PolyBase(parent),
	data(0)
{}

TestPolyObject::TestPolyObject(int data, QObject *parent) :
	PolyBase(parent),
	data(data)
{}
