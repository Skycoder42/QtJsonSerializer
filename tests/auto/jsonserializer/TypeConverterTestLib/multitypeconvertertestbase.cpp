#include "multitypeconvertertestbase.h"
#include <QtTest>
using namespace QtJsonSerializer;

MultiTypeConverterTestBase::MultiTypeConverterTestBase(QObject *parent) :
	TypeConverterTestBase{parent}
{}

TypeConverter *MultiTypeConverterTestBase::converter()
{
	QFETCH(TypeConverter*, converter);
	return converter;
}

void MultiTypeConverterTestBase::testConverterIsRegistered_data()
{
	QTest::addColumn<TypeConverter*>("converter");
	addConverterInstances();
}

void MultiTypeConverterTestBase::testConverterMeta_data()
{
	QTest::addColumn<TypeConverter*>("converter");
	TypeConverterTestBase::testConverterMeta_data();
}

void MultiTypeConverterTestBase::testMetaTypeDetection_data()
{
	QTest::addColumn<TypeConverter*>("converter");
	TypeConverterTestBase::testMetaTypeDetection_data();
}

void MultiTypeConverterTestBase::testSerialization_data()
{
	QTest::addColumn<TypeConverter*>("converter");
	TypeConverterTestBase::testSerialization_data();
}

void MultiTypeConverterTestBase::testDeserialization_data()
{
	QTest::addColumn<TypeConverter*>("converter");
	TypeConverterTestBase::testDeserialization_data();
}
