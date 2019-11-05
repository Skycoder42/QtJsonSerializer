#include "multitypeconvertertestbase.h"
#include <QtTest>
using namespace QtJsonSerializer;

MultiTypeConverterTestBase::MultiTypeConverterTestBase(QObject *parent) :
	TypeConverterTestBase{parent}
{}

QJsonTypeConverter *MultiTypeConverterTestBase::converter()
{
	QFETCH(QJsonTypeConverter*, converter);
	return converter;
}

void MultiTypeConverterTestBase::testConverterIsRegistered_data()
{
	QTest::addColumn<QJsonTypeConverter*>("converter");
	addConverterInstances();
}

void MultiTypeConverterTestBase::testConverterMeta_data()
{
	QTest::addColumn<QJsonTypeConverter*>("converter");
	TypeConverterTestBase::testConverterMeta_data();
}

void MultiTypeConverterTestBase::testMetaTypeDetection_data()
{
	QTest::addColumn<QJsonTypeConverter*>("converter");
	TypeConverterTestBase::testMetaTypeDetection_data();
}

void MultiTypeConverterTestBase::testSerialization_data()
{
	QTest::addColumn<QJsonTypeConverter*>("converter");
	TypeConverterTestBase::testSerialization_data();
}

void MultiTypeConverterTestBase::testDeserialization_data()
{
	QTest::addColumn<QJsonTypeConverter*>("converter");
	TypeConverterTestBase::testDeserialization_data();
}
