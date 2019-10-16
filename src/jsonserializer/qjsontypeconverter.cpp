#include "qjsontypeconverter.h"
#include "qjsonserializerbase_p.h"

class QJsonTypeConverterPrivate
{
public:
	int priority = QJsonTypeConverter::Standard;
};

QJsonTypeConverter::QJsonTypeConverter() :
	d{new QJsonTypeConverterPrivate{}}
{}

QJsonTypeConverter::~QJsonTypeConverter() = default;

int QJsonTypeConverter::priority() const
{
	return d->priority;
}

void QJsonTypeConverter::setPriority(int priority)
{
	d->priority = priority;
}

int QJsonTypeConverter::guessType(QCborTag tag, QCborValue::Type dataType) const
{
	Q_UNUSED(tag)
	Q_UNUSED(dataType)
	return QMetaType::UnknownType;
}

QJsonTypeConverter::DeserializationCapabilityResult QJsonTypeConverter::canDeserialize(int &metaTypeId, QCborTag tag, QCborValue::Type dataType, const SerializationHelper *helper) const
{
	const auto asJson = helper->jsonMode();
	const auto strict = helper->getProperty("validationFlags")
							.value<QJsonSerializerBase::ValidationFlags>()
							.testFlag(QJsonSerializerBase::ValidationFlag::StrictBasicTypes);

	// case A: a metaTypeId is present
	if (metaTypeId != QMetaType::UnknownType) {
		// first: verify the given metatype is supported
		if (!canConvert(metaTypeId))
			return DeserializationCapabilityResult::Negative;

		// second: verify the tag if not in json mode
		if (!asJson) {
			// if either we are in strict mode or a tag is given, the tag is verified
			if (strict || tag != NoTag) {
				// If there is a list of allowed tags, the given tag must be in it
				auto aTags = allowedCborTags(metaTypeId);
				// also, add the type specific override tag if set
				if (const auto xTag = helper->typeTag(metaTypeId); xTag != NoTag)
					aTags.append(xTag);
				if (!aTags.isEmpty()) {
					if (!aTags.contains(tag))
						return DeserializationCapabilityResult::WrongTag;
				// otherwise, if in strict mode, an empty allowed tag list means the tag must not be set
				} else if (strict && tag != NoTag)
					return DeserializationCapabilityResult::WrongTag;
			}
		}

		// third: verify the datatype, based on type and tag
		auto aTypes = allowedCborTypes(metaTypeId, tag);
		// if in json mode, convert the supported types to their json equivalent
		if (asJson)
			mapTypesToJson(aTypes);
		// then verify them
		if (!aTypes.contains(dataType))
			return DeserializationCapabilityResult::Negative;

		return DeserializationCapabilityResult::Positive;
	// case B: no metaTypeId is present, we are in cbor mode and have a tag
	} else if (!asJson && tag != NoTag){
		// try to guess the id from tag and type
		metaTypeId = guessType(tag, dataType);
		if (metaTypeId != QMetaType::UnknownType)
			return DeserializationCapabilityResult::Guessed;
		else
			return DeserializationCapabilityResult::Negative;
	// otherwise: cannot convert
	} else
		return DeserializationCapabilityResult::Negative;
}

QVariant QJsonTypeConverter::deserializeJson(int propertyType, const QCborValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	return deserializeCbor(propertyType, value, parent, helper);
}

QByteArray QJsonTypeConverter::getCanonicalTypeName(int propertyType) const
{
	return QJsonSerializerBasePrivate::getTypeName(propertyType);
}

void QJsonTypeConverter::mapTypesToJson(QList<QCborValue::Type> &typeList) const
{
	for (auto &type : typeList) {
		switch (type) {
		case QCborValue::Integer:
			type = QCborValue::Double;
			break;
		case QCborValue::ByteArray:
		case QCborValue::DateTime:
		case QCborValue::Url:
		case QCborValue::RegularExpression:
		case QCborValue::Uuid:
			type = QCborValue::String;
			break;
		default:
			break;
		}
	}
}



QJsonTypeConverter::SerializationHelper::SerializationHelper() = default;

QJsonTypeConverter::SerializationHelper::~SerializationHelper() = default;



QJsonTypeConverterFactory::QJsonTypeConverterFactory() = default;

QJsonTypeConverterFactory::~QJsonTypeConverterFactory() = default;

int QJsonTypeConverterFactory::priority() const
{
	if(!_statusConverter)
		_statusConverter = createConverter();
	return _statusConverter->priority();
}

bool QJsonTypeConverterFactory::canConvert(int metaTypeId) const
{
	if(!_statusConverter)
		_statusConverter = createConverter();
	return _statusConverter->canConvert(metaTypeId);
}

QJsonTypeConverter::DeserializationCapabilityResult QJsonTypeConverterFactory::canDeserialize(int &metaTypeId, QCborTag tag, QCborValue::Type dataType, const QJsonTypeConverter::SerializationHelper *helper) const
{
	if(!_statusConverter)
		_statusConverter = createConverter();
	return _statusConverter->canDeserialize(metaTypeId, tag, dataType, helper);
}
