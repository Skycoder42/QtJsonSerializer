#ifndef MULTITYPECONVERTERTESTBASE_H
#define MULTITYPECONVERTERTESTBASE_H

#include "typeconvertertestbase.h"

class MultiTypeConverterTestBase : public TypeConverterTestBase
{
	Q_OBJECT

public:
	explicit MultiTypeConverterTestBase(QObject *parent = nullptr);

protected:
	QJsonTypeConverter *converter() final;
	void testConverterMeta_data() override;
	void testMetaTypeDetection_data() override;
	void testSerialization_data() override;
	void testDeserialization_data() override;
};

Q_DECLARE_METATYPE(QJsonTypeConverter*)

#endif // MULTITYPECONVERTERTESTBASE_H
