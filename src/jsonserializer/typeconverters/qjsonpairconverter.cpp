#include "qjsonpairconverter_p.h"
#include "qjsonserializerexception.h"
#include <QCborSerializer>

#include <QtCore/QCborArray>

const QRegularExpression QJsonPairConverter::pairTypeRegex(QStringLiteral(R"__(^(?:QPair|std::pair)<(.*?)>$)__"));

bool QJsonPairConverter::canConvert(int metaTypeId) const
{
	return pairTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(metaTypeId))).hasMatch();
}

QList<QCborTag> QJsonPairConverter::allowedCborTags(int metaTypeId) const
{
	Q_UNUSED(metaTypeId)
	return {NoTag, static_cast<QCborTag>(QCborSerializer::Pair)};
}

QList<QCborValue::Type> QJsonPairConverter::allowedCborTypes(int metaTypeId, QCborTag tag) const
{
	Q_UNUSED(metaTypeId)
	Q_UNUSED(tag)
	return {QCborValue::Array};
}

QCborValue QJsonPairConverter::serialize(int propertyType, const QVariant &value) const
{
	const auto types = getPairTypes(propertyType);
	const auto targetType = qMetaTypeId<QPair<QVariant, QVariant>>();
	auto cValue = value;
	if (!cValue.canConvert(targetType) || !cValue.convert(targetType)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to QPair<QVariant, QVariant>. Make shure to register pair types via QJsonSerializer::registerPairConverters"));
	}

	auto variant = cValue.value<QPair<QVariant, QVariant>>();
	return {
		static_cast<QCborTag>(QCborSerializer::Pair),
		QCborArray {
			helper()->serializeSubtype(types.first, variant.first, "first"),
			helper()->serializeSubtype(types.second, variant.second, "second")
		}
	};
}

QVariant QJsonPairConverter::deserializeCbor(int propertyType, const QCborValue &value, QObject *parent) const
{
	const auto types = getPairTypes(propertyType);
	const auto array = (value.isTag() ? value.taggedValue() : value).toArray();
	if (array.size() != 2)
		throw QJsonDeserializationException("CBOR/JSON array must have exactly 2 elements to be read as a pair");

	QPair<QVariant, QVariant> vPair;
	vPair.first = helper()->deserializeSubtype(types.first, array[0], parent, "first");
	vPair.second = helper()->deserializeSubtype(types.second, array[1], parent, "second");
	return QVariant::fromValue(vPair);
}

std::pair<int, int> QJsonPairConverter::getPairTypes(int metaType) const
{
	auto match = pairTypeRegex.match(QString::fromUtf8(helper()->getCanonicalTypeName(metaType)));
	if (match.hasMatch()) {
		// parse match, using <> and , rules
		const auto matchStr = match.captured(1);
		auto bCount = 0;
		for (auto i = 0; i < matchStr.size(); ++i) {
			const auto &c = matchStr[i];
			if (c == QLatin1Char('<'))
				++bCount;
			else if (c == QLatin1Char('>'))
				--bCount;
			else if (bCount == 0 && c == QLatin1Char(',')) {
				return std::make_pair(QMetaType::type(matchStr.mid(0, i).trimmed().toUtf8()),
									  QMetaType::type(matchStr.mid(i + 1).trimmed().toUtf8()));
			}
		}
	}

	return std::make_pair(QMetaType::UnknownType, QMetaType::UnknownType);
}
