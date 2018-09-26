#include "qtjsonserializer_global.h"
#include "qjsonserializer.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QUrl>
#include <QtCore/QVersionNumber>
#include <QtCore/QBuffer>
#include <QtCore/QLine>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QLocale>
#include <QtCore/QRegularExpression>

static void qJsonSerializerStartup();
Q_COREAPP_STARTUP_FUNCTION(qJsonSerializerStartup)

//use macro to register types to reduce number of symbols generated
#define REGISTER_DEFAULT_LIST(T) \
	ok = QMetaType::registerConverter<QList<T>, QVariantList>([](const QList<T> &list) -> QVariantList { \
		QVariantList l; \
		l.reserve(list.size()); \
		for(const auto &v : list) \
			l.append(QVariant::fromValue(v)); \
		return l; \
	}); \
	Q_ASSERT_X(ok, Q_FUNC_INFO, "Failed to register QList<T> -> QVariantList converter for type " #T); \
	\
	ok = QMetaType::registerConverter<QVariantList, QList<T>>([](const QVariantList &list) -> QList<T> { \
		QList<T> l; \
		l.reserve(list.size()); \
		for(auto v : list) { \
			auto vt = v.userType(); \
			if(v.convert(qMetaTypeId<T>())) \
				l.append(v.value<T>()); \
			else { \
				qWarning() << "Conversion to" \
						   << QMetaType::typeName(qMetaTypeId<QList<T>>()) \
						   << "failed, could not convert element of type" \
						   << QMetaType::typeName(vt); \
				l.append(T()); \
			} \
		} \
		return l; \
	}); \
	Q_ASSERT_X(ok, Q_FUNC_INFO, "Failed to register QVariantList -> QList<T> converter for type " #T);

#define REGISTER_DEFAULT_MAP(T) \
	ok = QMetaType::registerConverter<QMap<QString, T>, QVariantMap>([](const QMap<QString, T> &map) -> QVariantMap { \
		QVariantMap m; \
		for(auto it = map.constBegin(); it != map.constEnd(); ++it) \
			m.insert(it.key(), QVariant::fromValue(it.value())); \
		return m; \
	}); \
	Q_ASSERT_X(ok, Q_FUNC_INFO, "Failed to register QMap<QString, T> -> QVariantMap converter for type " #T); \
	\
	ok = QMetaType::registerConverter<QVariantMap, QMap<QString, T>>([](const QVariantMap &map) -> QMap<QString, T> { \
		QMap<QString, T> m; \
		for(auto it = map.constBegin(); it != map.constEnd(); ++it) { \
			auto v = it.value(); \
			auto vt = v.userType(); \
			if(v.convert(qMetaTypeId<T>())) \
				m.insert(it.key(), v.value<T>()); \
			else { \
				qWarning() << "Conversion to" \
						   << QMetaType::typeName(qMetaTypeId<QMap<QString, T>>()) \
						   << "failed, could not convert element value of type" \
						   << QMetaType::typeName(vt); \
				m.insert(it.key(), T()); \
			} \
		} \
		return m; \
	}); \
	Q_ASSERT_X(ok, Q_FUNC_INFO, "Failed to register QVariantMap -> QMap<QString, T> converter for type " #T);

#ifdef Q_JSONSERIALIZER_NO_MACRO
#define REGISTER_DEFAULT_CONVERTERS(T) QJsonSerializer::registerAllConverters<T>()
#else
#define REGISTER_DEFAULT_CONVERTERS(T) do { \
	bool ok = false; \
	REGISTER_DEFAULT_LIST(T) \
	REGISTER_DEFAULT_MAP(T) \
} while(false)
#endif

static void qJsonSerializerStartup()
{
	//primitive types
	REGISTER_DEFAULT_CONVERTERS(bool);
	REGISTER_DEFAULT_CONVERTERS(int);
	REGISTER_DEFAULT_CONVERTERS(uint);
	REGISTER_DEFAULT_CONVERTERS(qlonglong);
	REGISTER_DEFAULT_CONVERTERS(qulonglong);
	REGISTER_DEFAULT_CONVERTERS(double);
	REGISTER_DEFAULT_CONVERTERS(long);
	REGISTER_DEFAULT_CONVERTERS(short);
	REGISTER_DEFAULT_CONVERTERS(char);
	REGISTER_DEFAULT_CONVERTERS(ulong);
	REGISTER_DEFAULT_CONVERTERS(ushort);
	REGISTER_DEFAULT_CONVERTERS(uchar);
	REGISTER_DEFAULT_CONVERTERS(float);
	//macro does not work for the following two
	QJsonSerializer::registerAllConverters<signed char>();
	QJsonSerializer::registerAllConverters<QObject*>();

	//core types
	REGISTER_DEFAULT_CONVERTERS(QChar);
	REGISTER_DEFAULT_CONVERTERS(QString);
	REGISTER_DEFAULT_CONVERTERS(QDate);
	REGISTER_DEFAULT_CONVERTERS(QTime);
	REGISTER_DEFAULT_CONVERTERS(QDateTime);
	REGISTER_DEFAULT_CONVERTERS(QUrl);
	REGISTER_DEFAULT_CONVERTERS(QUuid);
	REGISTER_DEFAULT_CONVERTERS(QJsonValue);
	REGISTER_DEFAULT_CONVERTERS(QJsonObject);
	REGISTER_DEFAULT_CONVERTERS(QJsonArray);
	REGISTER_DEFAULT_CONVERTERS(QVersionNumber);
	REGISTER_DEFAULT_CONVERTERS(QSize);
	REGISTER_DEFAULT_CONVERTERS(QPoint);
	REGISTER_DEFAULT_CONVERTERS(QLine);
	REGISTER_DEFAULT_CONVERTERS(QRect);
	REGISTER_DEFAULT_CONVERTERS(QLocale);
	REGISTER_DEFAULT_CONVERTERS(QRegularExpression);

	//extra: qbytearray
	{
		bool ok = false;
//		REGISTER_DEFAULT_LIST(QByteArray)
		REGISTER_DEFAULT_MAP(QByteArray)
	}
}

