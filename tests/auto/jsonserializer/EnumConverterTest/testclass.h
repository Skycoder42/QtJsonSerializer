#ifndef TESTCLASS_H
#define TESTCLASS_H

#include <QObject>

class TestClass : public QObject
{
	Q_OBJECT

public:
	enum class TestEnum {
		Key0,
		Key1,
		Key4 = 4
	};
	Q_ENUM(TestEnum)

	enum class TestFlag {
		Flag1 = 0x01,
		Flag2 = 0x02,
		Flag4 = 0x04
	};
	Q_DECLARE_FLAGS(TestFlags, TestFlag)
	Q_FLAG(TestFlags)

	explicit TestClass(QObject *parent = nullptr);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TestClass::TestFlags)

#endif // TESTCLASS_H
