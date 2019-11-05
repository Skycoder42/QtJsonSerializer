#include "exceptioncontext_p.h"

#include <QtCore/qdebug.h>
using namespace QtJsonSerializer;

QThreadStorage<SerializationException::PropertyTrace> ExceptionContext::contextStore;

ExceptionContext::ExceptionContext(const QMetaProperty &property)
{
	contextStore.localData().push({
									  property.name(),
									  property.isEnumType() ?
										 property.enumerator().name() :
										 property.typeName()
								  });
}

ExceptionContext::ExceptionContext(int propertyType, const QByteArray &hint)
{
	contextStore.localData().push({
									  hint.isNull() ? QByteArray("<unnamed>") : hint,
									  QMetaType::typeName(propertyType)
								  });
}

ExceptionContext::~ExceptionContext()
{
	auto &context = contextStore.localData();
	if(context.isEmpty())
		qWarning() << "Corrupted context store";
	else
		context.pop();
}

SerializationException::PropertyTrace ExceptionContext::currentContext()
{
	return contextStore.localData();
}
