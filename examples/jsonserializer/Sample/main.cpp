#include <QCoreApplication>
#include <QtJsonSerializer>
#include <QDebug>
using namespace QtJsonSerializer;

#include "sampleobject.h"

#define JSON_PRINT(x) QJsonDocument(x).toJson().constData()

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	//to actually work polymorphic, we must register:
	qRegisterMetaType<SuperSampleObject*>();

	QJsonSerializer serializer;

	qDebug() << "Serializing SampleGadget:";
	SampleGadget gadget;
	gadget.base = {42, 13};
	gadget.rawData = QJsonObject({
									 {QStringLiteral("someData"), false}
								 });
	auto json = serializer.serialize(gadget);
	qDebug() << JSON_PRINT(json);

	qDebug() << "\nAnd deserializing data back:";
	auto deser = serializer.deserialize<SampleGadget>(json);
	qDebug() << "base:" << deser.base;
	qDebug() << "rawData:" << JSON_PRINT(deser.rawData);

	qDebug() << "\nNow the test object, with the previous gadget as child:";
	auto obj = new SampleObject(qApp);
	obj->id = 10;
	obj->title = QStringLiteral("Example");
	obj->flags = SampleObject::ValueA | SampleObject::ValueB;
	obj->scores = {0.0, 6.66, 47.11};
	obj->secret = QStringLiteral("Very secret string, not serialized");
	obj->gadget = gadget;
	json = serializer.serialize(obj);
	qDebug() << JSON_PRINT(json);

	qDebug() << "\nAnd serialized with string flags and a child";
	serializer.setEnumAsString(true);
	obj->child = new SampleObject(obj);
	json = serializer.serialize(obj);
	qDebug() << JSON_PRINT(json);

	qDebug() << "\nSerialize a polymorphic object type now:";
	obj = new SuperSampleObject(qApp);
	obj->id = 55;
	obj->title = QStringLiteral("Super Example");
	json = serializer.serialize(obj);
	qDebug() << JSON_PRINT(json);

	qDebug() << "\nFinally, deserialize the polymorphic type again";
	obj = serializer.deserialize<SampleObject*>(json);//serialize as SampleObject
	qDebug() << "SampleObject pointer to:" << obj;

	return 0;
}
