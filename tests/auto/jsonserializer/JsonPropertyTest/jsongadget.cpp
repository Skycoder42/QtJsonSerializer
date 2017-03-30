#include "jsongadget.h"

JsonGadget::JsonGadget() :
	object(),
	array(),
	value(QJsonValue::Null)
{}

JsonGadget::JsonGadget(QJsonObject object) :
	object(object),
	array(),
	value(QJsonValue::Null)
{}

JsonGadget::JsonGadget(QJsonArray array) :
	object(),
	array(array),
	value(QJsonValue::Null)
{}

JsonGadget::JsonGadget(QJsonValue value) :
	object(),
	array(),
	value(value)
{}

bool JsonGadget::operator ==(const JsonGadget &other) const
{
	return object == other.object &&
			array == other.array &&
			value == other.value;
}
