ALL_TYPES = \
	bool \
	int \
	uint \
	qlonglong \
	qulonglong \
	double \
	long \
	short \
	char \
	"signed char" \
	ulong \
	ushort \
	uchar \
	float \
	QObject* \
	QChar \
	QString \
	QDate \
	QTime \
	QDateTime \
	QUrl \
	QUuid \
	QJsonValue \
	QJsonObject \
	QJsonArray \
	QVersionNumber \
	QSize \
	QPoint \
	QLine \
	QRect \
	QLocale \
	QRegularExpression
LIST_TYPES =
MAP_TYPES = \
	QByteArray

isEmpty(QT_JSONSERIALIZER_REGPATH): QT_JSONSERIALIZER_REGPATH = $$OUT_PWD/.reggen
mkpath($$QT_JSONSERIALIZER_REGPATH)
type_index = 0

ALL_TYPES.method = registerAllConverters
ALL_TYPES.desc = "list and map"

LIST_TYPES.method = registerListConverters
LIST_TYPES.desc = "map"

MAP_TYPES.method = registerMapConverters
MAP_TYPES.desc = "list"

for(tId, $$list(ALL_TYPES, LIST_TYPES, MAP_TYPES)) {
	for(type, $$tId) {
		raw_data = $$cat($$PWD/qjsonconverterreg.cpp.template, blob)
		raw_data = $$replace(raw_data, $$re_escape("%{typeindex}"), $$type_index)
		raw_data = $$replace(raw_data, $$re_escape("%{convertMethod}"), $$first($${tId}.method))
		raw_data = $$replace(raw_data, $$re_escape("%{type}"), "$$type")
		raw_data = $$replace(raw_data, $$re_escape("%{convertOp}"), $$first($${tId}.desc))

		out_file = $$QT_JSONSERIALIZER_REGPATH/qjsonconverterreg_$${type_index}.cpp
		!exists($$out_file):!write_file($$out_file, raw_data):error("Failed to create $$out_file")
		type_index = $$num_add($$type_index, 1)

		GENERATED_SOURCES += $$out_file
		export(GENERATED_SOURCES)
	}
}

DISTFILES += \
	$$PWD/qjsonconverterreg.cpp.template
