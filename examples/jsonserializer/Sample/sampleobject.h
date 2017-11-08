#ifndef SAMPLEOBJECT_H
#define SAMPLEOBJECT_H

#include <QObject>
#include "samplegadget.h"

class SampleObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(QString title MEMBER title)
	Q_PROPERTY(SuperFlags flags READ getFlags WRITE setFlags)
	Q_PROPERTY(QList<double> scores MEMBER scores)
	Q_PROPERTY(SampleObject* child MEMBER child)
	Q_PROPERTY(SampleGadget gadget MEMBER gadget)
	Q_PROPERTY(QString secret MEMBER secret STORED false)

public:
	enum SuperFlag {
		ValueA = 0x01,
		ValueB = 0x02,
		ValueC = 0x04
	};
	Q_DECLARE_FLAGS(SuperFlags, SuperFlag)
	Q_FLAG(SuperFlags)

	Q_INVOKABLE SampleObject(QObject *parent = nullptr);

	int id;
	QString title;
	SuperFlags flags;
	QList<double> scores;
	SampleObject *child;
	SampleGadget gadget;
	QString secret;

private:
	SuperFlags getFlags() const;
	void setFlags(const SuperFlags &value);
};

class SuperSampleObject : public SampleObject
{
	Q_OBJECT
	Q_CLASSINFO("polymorphic", "true")

	Q_PROPERTY(bool working MEMBER working)

public:
	Q_INVOKABLE SuperSampleObject(QObject *parent = nullptr);

	bool working;
};

Q_DECLARE_METATYPE(SampleObject*)
Q_DECLARE_METATYPE(SuperSampleObject*)
Q_DECLARE_OPERATORS_FOR_FLAGS(SampleObject::SuperFlags)

#endif // SAMPLEOBJECT_H
