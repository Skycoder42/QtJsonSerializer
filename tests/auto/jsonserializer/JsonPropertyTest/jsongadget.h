#ifndef JSONGADGET_H
#define JSONGADGET_H

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

class JsonGadget
{
	Q_GADGET

	Q_PROPERTY(QJsonObject object MEMBER object)
	Q_PROPERTY(QJsonArray array MEMBER array)
	Q_PROPERTY(QJsonValue value MEMBER value)

public:
	JsonGadget();
	JsonGadget(QJsonObject object);
	JsonGadget(QJsonArray array);
	JsonGadget(QJsonValue value);

	QJsonObject object;
	QJsonArray array;
	QJsonValue value;

	bool operator ==(const JsonGadget &other) const;
};

Q_DECLARE_METATYPE(JsonGadget)

#endif // JSONGADGET_H
