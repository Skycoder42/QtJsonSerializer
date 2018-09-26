#include "sampleobject.h"

SampleObject::SampleObject(QObject *parent) :
	QObject{parent}
{}

SampleObject::SuperFlags SampleObject::getFlags() const
{
	return flags;
}

void SampleObject::setFlags(SuperFlags value)
{
	flags = value;
}

SuperSampleObject::SuperSampleObject(QObject *parent) :
	SampleObject(parent),
	working(true)
{}
