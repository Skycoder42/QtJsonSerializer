#ifndef TESTCONVERTER_H
#define TESTCONVERTER_H

#include <QtCore/QObject>
#include <QtJsonSerializer/QJsonTypeConverter>

class EnumContainer
{
	Q_GADGET

	Q_PROPERTY(NormalEnum normalEnum MEMBER normalEnum)
	Q_PROPERTY(EnumFlags enumFlags READ getEnumFlags WRITE setEnumFlags)

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

	NormalEnum normalEnum;
	EnumFlags enumFlags;

	bool operator==(EnumContainer other) const;
	bool operator!=(EnumContainer other) const;

private:
	EnumFlags getEnumFlags() const;
	void setEnumFlags(EnumFlags value);
};

class TestEnumConverter : public QtJsonSerializer::QJsonTypeConverter
{
public:
	using NormalEnum = EnumContainer::NormalEnum;
	using EnumFlags = EnumContainer::EnumFlags;

	TestEnumConverter();

	bool canConvert(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;
};

class TestWrapperConverter : public QtJsonSerializer::QJsonTypeConverter
{
public:
	TestWrapperConverter();

	bool canConvert(int metaTypeId) const override;
	QList<QCborValue::Type> allowedCborTypes(int metaTypeId, QCborTag tag) const override;
	QCborValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const override;
};

class TestObject : public QObject
{
	Q_OBJECT

public:
	TestObject(QObject *parent = nullptr);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EnumContainer::EnumFlags)

Q_DECLARE_METATYPE(EnumContainer)

#endif // TESTCONVERTER_H
