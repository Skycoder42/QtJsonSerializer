#ifndef SAMPLEGADGET_H
#define SAMPLEGADGET_H

#include <QJsonObject>
#include <QObject>
#include <QPoint>

class SampleGadget
{
	Q_GADGET

	Q_PROPERTY(QPoint base MEMBER base)
	Q_PROPERTY(QJsonObject rawData MEMBER rawData)

public:
	SampleGadget();

	QPoint base;
	QJsonObject rawData;

	bool operator !=(const SampleGadget &other) const;
};

Q_DECLARE_METATYPE(SampleGadget)

#endif // SAMPLEGADGET_H
