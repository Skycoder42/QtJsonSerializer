#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <QtJsonSerializer/QJsonSerializer>

class TestObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int key MEMBER key)
	Q_PROPERTY(double value MEMBER value)
	Q_PROPERTY(int zhidden MEMBER zhidden STORED false)

public:
	Q_INVOKABLE explicit TestObject(QObject *parent = nullptr);
	Q_INVOKABLE explicit TestObject(int key, double value, int zhidden = 11, QObject *parent = nullptr);

	int key = 0;
	double value = 0;
	int zhidden = 11;

	virtual bool compare(const TestObject *other) const;
};

class StaticPolyObject : public TestObject
{
	Q_OBJECT
	Q_JSON_POLYMORPHIC(true)

	Q_PROPERTY(bool extra1 MEMBER extra1)

public:
	Q_INVOKABLE explicit StaticPolyObject(QObject *parent = nullptr);
	explicit StaticPolyObject(int key, double value, int hidden, bool extra1, QObject *parent = nullptr);

	bool extra1 = false;

	bool compare(const TestObject *other) const override;
};

class StaticNonPolyObject : public TestObject
{
	Q_OBJECT
	Q_JSON_POLYMORPHIC(false)

	Q_PROPERTY(bool extra2 MEMBER extra2)

public:
	Q_INVOKABLE explicit StaticNonPolyObject(QObject *parent = nullptr);
	explicit StaticNonPolyObject(int key, double value, int hidden, bool extra2, QObject *parent = nullptr);

	bool extra2 = false;

	bool compare(const TestObject *other) const override;
};

class DynamicPolyObject : public TestObject
{
	Q_OBJECT

	Q_PROPERTY(bool extra3 MEMBER extra3)

public:
	Q_INVOKABLE explicit DynamicPolyObject(QObject *parent = nullptr);
	explicit DynamicPolyObject(int key, double value, int hidden, bool extra3, bool poly, QObject *parent = nullptr);

	bool extra3 = false;

	bool compare(const TestObject *other) const override;
};

class DerivedTestObject : public TestObject
{
	Q_OBJECT

	Q_PROPERTY(bool extra4 MEMBER extra4)

public:
	Q_INVOKABLE explicit DerivedTestObject(QObject *parent = nullptr);
	explicit DerivedTestObject(int key, double value, int hidden, bool extra4, QObject *parent = nullptr);

	bool extra4 = false;

	bool compare(const TestObject *other) const override;
};

class BrokenObject : public TestObject
{
	Q_OBJECT

public:
	explicit BrokenObject(QObject *parent = nullptr);
};

Q_DECLARE_METATYPE(TestObject*)
Q_DECLARE_METATYPE(StaticPolyObject*)
Q_DECLARE_METATYPE(StaticNonPolyObject*)
Q_DECLARE_METATYPE(DynamicPolyObject*)
Q_DECLARE_METATYPE(DerivedTestObject*)
Q_DECLARE_METATYPE(BrokenObject*)

#endif // TESTOBJECT_H
