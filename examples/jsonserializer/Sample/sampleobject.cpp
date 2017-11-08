#include "sampleobject.h"

SampleObject::SampleObject(QObject *parent) :
	QObject(parent),
	id(0),
	title(),
	flags(0),
	scores(),
	child(nullptr),
	gadget(),
	secret()
{}

SampleObject::SuperFlags SampleObject::getFlags() const
{
	return flags;
}

void SampleObject::setFlags(const SuperFlags &value)
{
	flags = value;
}

SuperSampleObject::SuperSampleObject(QObject *parent) :
	SampleObject(parent),
	working(true)
{}
