#include "qjsonexceptioncontext_p.h"

QThreadStorage<QJsonSerializationException::PropertyTrace> QJsonExceptionContext::contextStore;

QJsonExceptionContext::QJsonExceptionContext(const QMetaProperty &property)
{
	contextStore.localData().enqueue({
										 property.name(),
										 property.isEnumType() ?
											property.enumerator().name() :
											property.typeName()
									 });
}

QJsonExceptionContext::QJsonExceptionContext(int propertyType, const QByteArray &hint)
{
	contextStore.localData().enqueue({
										 hint.isNull() ? QByteArray("<unnamed>") : hint,
										 QMetaType::typeName(propertyType)
									 });
}

QJsonExceptionContext::~QJsonExceptionContext()
{
	auto &context = contextStore.localData();
	if(context.isEmpty())
		qWarning() << "Corrupted context store";
	else
		context.dequeue();
}

QJsonSerializationException::PropertyTrace QJsonExceptionContext::currentContext()
{
	return contextStore.localData();
}
