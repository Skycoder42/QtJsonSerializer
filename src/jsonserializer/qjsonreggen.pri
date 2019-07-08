DISTFILES += \
	$$PWD/qjsonreggen.py

JSON_TYPES = \
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
	QLocale \
	QRegularExpression \
	QSize \
	QPoint \
	QLine \
	QRect \
	QByteArray

QSize.modes = list
QPoint.modes = list
QLine.modes = list
QRect.modes = list
QByteArray.modes = map set

isEmpty(QT_JSONSERIALIZER_REGGEN_DIR): QT_JSONSERIALIZER_REGGEN_DIR = $$OUT_PWD/.reggen
debug_and_release {
	CONFIG(debug, debug|release): QT_JSONSERIALIZER_REGGEN_DIR = $$QT_JSONSERIALIZER_REGGEN_DIR/debug
	CONFIG(release, debug|release): QT_JSONSERIALIZER_REGGEN_DIR = $$QT_JSONSERIALIZER_REGGEN_DIR/release
}

isEmpty(QT_JSONSERIALIZER_TYPESPLIT_PY) {
	win32: QT_JSONSERIALIZER_TYPESPLIT_PY = python
	QT_JSONSERIALIZER_TYPESPLIT_PY += $$shell_path($$PWD/qjsonreggen.py)
}

reggen_dir.target = $$QT_JSONSERIALIZER_REGGEN_DIR
reggen_dir.commands = $$QMAKE_MKDIR $$shell_quote($$shell_path($$QT_JSONSERIALIZER_REGGEN_DIR))
reggen_dir.depends = $$PWD/qjsonreggen.pri
QMAKE_EXTRA_TARGETS += reggen_dir

for(type, JSON_TYPES) {
	type_base = $$replace(type, "\\W", "_")
	target_base = qjsonconverterreg_$${type_base}.cpp
	target_path = $$absolute_path($$target_base, $$QT_JSONSERIALIZER_REGGEN_DIR)
	$${target_path}.name = $$target_path
	$${target_path}.depends = $$PWD/qjsonreggen.py $$PWD/qjsonreggen.pri $$QT_JSONSERIALIZER_REGGEN_DIR
	$${target_path}.commands = $$QT_JSONSERIALIZER_TYPESPLIT_PY $$shell_quote($$target_path) $$shell_quote($$type) $$eval($${type_base}.modes)
	QMAKE_EXTRA_TARGETS += $$target_path
	GENERATED_SOURCES += $$target_path
}

escaped_types =
for(type, JSON_TYPES): escaped_types += $$shell_quote($$type)
target_path = $$absolute_path(qjsonconverterreg_hook.cpp, $$QT_JSONSERIALIZER_REGGEN_DIR)
$${target_path}.name = $$target_path
$${target_path}.depends = $$PWD/qjsonreggen.py $$PWD/qjsonreggen.pri $$QT_JSONSERIALIZER_REGGEN_DIR
$${target_path}.commands = $$QT_JSONSERIALIZER_TYPESPLIT_PY super $$shell_quote($$target_path) $$escaped_types
QMAKE_EXTRA_TARGETS += $$target_path
GENERATED_SOURCES += $$target_path
