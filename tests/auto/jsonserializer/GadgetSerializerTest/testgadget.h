#ifndef TESTGADGET_H
#define TESTGADGET_H

#include <QObject>
#include <QDateTime>
#include <QUuid>
#include <QUrl>
#include <QList>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QVersionNumber>
#include <QSize>
#include <QPoint>
#include <QLine>
#include <QRect>
#include <QLocale>
#include <QRegularExpression>

struct ChildGadget
{
	Q_GADGET

	Q_PROPERTY(int data MEMBER data)

public:
	int data;

	ChildGadget(int data = 0);

	bool operator==(const ChildGadget &other) const;
	bool operator!=(const ChildGadget &other) const;
	bool operator<(const ChildGadget &other) const;

	static QJsonObject createJson(const int &data = 0);
};

struct TestGadget
{
	Q_GADGET

	Q_PROPERTY(int intProperty MEMBER intProperty)
	Q_PROPERTY(bool boolProperty MEMBER boolProperty)
	Q_PROPERTY(QString stringProperty MEMBER stringProperty)
	Q_PROPERTY(double doubleProperty MEMBER doubleProperty)

	Q_PROPERTY(NormalEnum normalEnumProperty MEMBER normalEnumProperty)
	Q_PROPERTY(EnumFlags enumFlagsProperty READ getEnumFlagsProperty WRITE setEnumFlagsProperty)

	Q_PROPERTY(QDateTime datetime MEMBER datetime)
	Q_PROPERTY(QUuid uuid MEMBER uuid)
	Q_PROPERTY(QUrl url MEMBER url)
	Q_PROPERTY(QVersionNumber version MEMBER version)
	Q_PROPERTY(QByteArray bytearray MEMBER bytearray)

	Q_PROPERTY(QSize size MEMBER size)
	Q_PROPERTY(QPoint point MEMBER point)
	Q_PROPERTY(QLine line MEMBER line)
	Q_PROPERTY(QRect rect MEMBER rect)

	Q_PROPERTY(QLocale locale MEMBER locale)
	Q_PROPERTY(QRegularExpression regexp MEMBER regexp)

	Q_PROPERTY(QList<int> simpleList MEMBER simpleList)
	Q_PROPERTY(QList<QList<int>> leveledList MEMBER leveledList)

	Q_PROPERTY(QMap<QString, int> simpleMap MEMBER simpleMap)
	Q_PROPERTY(QMap<QString, QMap<QString, int>> leveledMap MEMBER leveledMap)

	Q_PROPERTY(QPair<int, QString> pair MEMBER pair)
	Q_PROPERTY(QPair<ChildGadget, QList<int>> extraPair MEMBER extraPair)
	Q_PROPERTY(QList<QPair<bool, bool>> listPair MEMBER listPair)

	Q_PROPERTY(ChildGadget childGadget MEMBER childGadget)

	Q_PROPERTY(QList<ChildGadget> simpleChildren MEMBER simpleChildren)
	Q_PROPERTY(QList<QList<ChildGadget>> leveledChildren MEMBER leveledChildren)

	Q_PROPERTY(QMap<QString, ChildGadget> simpleRelatives MEMBER simpleRelatives)
	Q_PROPERTY(QMap<QString, QMap<QString, ChildGadget>> leveledRelatives MEMBER leveledRelatives)

	Q_PROPERTY(QJsonObject object MEMBER object)
	Q_PROPERTY(QJsonArray array MEMBER array)
	Q_PROPERTY(QJsonValue value MEMBER value)

public:
	enum NormalEnum {
		Normal0 = 0,
		Normal1 = 1,
		Normal2 = 2
	};
	Q_ENUM(NormalEnum)
	enum EnumFlag {
		Flag1 = 0x02,
		Flag2 = 0x04,
		Flag3 = 0x08,

		FlagX = Flag1 | Flag2
	};
	Q_DECLARE_FLAGS(EnumFlags, EnumFlag)
	Q_FLAG(EnumFlags)

	TestGadget();

	bool operator==(const TestGadget &other) const;
	bool operator!=(const TestGadget &other) const;
	bool operator<(const TestGadget &other) const;

	static TestGadget createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty);
	static TestGadget createEnum(NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty);
	static TestGadget createExtra(QDateTime datetime, QUuid uuid, QUrl url, QVersionNumber version, QByteArray bytearray);
	static TestGadget createGeom(QSize size, QPoint point, QLine line, QRect rect);
	static TestGadget createSpecial(QLocale locale, QRegularExpression regexp);
	static TestGadget createList(QList<int> simpleList, QList<QList<int>> leveledList);
	static TestGadget createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int>> leveledMap);
	static TestGadget createPair(QPair<int, QString> pair, QPair<ChildGadget, QList<int>> extraPair, QList<QPair<bool, bool>> listPair);
	static TestGadget createChild(ChildGadget childGadget);
	static TestGadget createChildren(QList<ChildGadget> simpleChildren, QList<QList<ChildGadget>> leveledChildren);
	static TestGadget createRelatives(QMap<QString, ChildGadget> simpleRelatives, QMap<QString, QMap<QString, ChildGadget>> leveledRelatives);
	static TestGadget createEmbedded(QJsonObject object, QJsonArray array, QJsonValue value);

	static QJsonObject createJson(const QJsonObject &delta = QJsonObject(), const QString &rmKey = {});

	int intProperty;
	bool boolProperty;
	QString stringProperty;
	double doubleProperty;

	NormalEnum normalEnumProperty;
	EnumFlags enumFlagsProperty;

	QDateTime datetime;
	QUuid uuid;
	QUrl url;
	QVersionNumber version;
	QByteArray bytearray;

	QSize size;
	QPoint point;
	QLine line;
	QRect rect;

	QLocale locale;
	QRegularExpression regexp;

	QList<int> simpleList;
	QList<QList<int>> leveledList;

	QMap<QString, int> simpleMap;
	QMap<QString, QMap<QString, int>> leveledMap;

	QPair<int, QString> pair;
	QPair<ChildGadget, QList<int>> extraPair;
	QList<QPair<bool, bool>> listPair;

	ChildGadget childGadget;

	QList<ChildGadget> simpleChildren;
	QList<QList<ChildGadget>> leveledChildren;

	QMap<QString, ChildGadget> simpleRelatives;
	QMap<QString, QMap<QString, ChildGadget>> leveledRelatives;

	QJsonObject object;
	QJsonArray array;
	QJsonValue value;

	EnumFlags getEnumFlagsProperty() const;
	void setEnumFlagsProperty(const EnumFlags &value);
};

Q_DECLARE_METATYPE(TestGadget)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestGadget::EnumFlags)

#endif // TESTGADGET_H
