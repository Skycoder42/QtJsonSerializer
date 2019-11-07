#include "exceptioncontext_p.h"
using namespace QtJsonSerializer;

Q_LOGGING_CATEGORY(QtJsonSerializer::logExceptCtx, "qt.jsonserializer.private.exceptioncontext")

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
	if (context.isEmpty())
		qCWarning(logExceptCtx) << "Corrupted context store";
	else
		context.pop();
}

SerializationException::PropertyTrace ExceptionContext::currentContext()
{
	return contextStore.localData();
}

int ExceptionContext::currentDepth()
{
	return contextStore.localData().size();
}
