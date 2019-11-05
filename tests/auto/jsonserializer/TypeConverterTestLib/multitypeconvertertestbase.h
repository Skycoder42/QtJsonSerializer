#ifndef MULTITYPECONVERTERTESTBASE_H
#define MULTITYPECONVERTERTESTBASE_H

#include "typeconvertertestbase.h"

class MultiTypeConverterTestBase : public TypeConverterTestBase
{
	Q_OBJECT

public:
	explicit MultiTypeConverterTestBase(QObject *parent = nullptr);

protected:
	QtJsonSerializer::QJsonTypeConverter *converter() final;
	void testConverterIsRegistered_data() override;
	void testConverterMeta_data() override;
	void testMetaTypeDetection_data() override;
	void testSerialization_data() override;
	void testDeserialization_data() override;

	virtual void addConverterInstances() = 0;
};

Q_DECLARE_METATYPE(QtJsonSerializer::QJsonTypeConverter*)

#endif // MULTITYPECONVERTERTESTBASE_H
