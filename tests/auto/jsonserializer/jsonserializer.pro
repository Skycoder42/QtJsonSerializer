TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest \
	TypeConverterTestLib

CONVERTER_TESTS = \
	BytearrayConverterTest \
	GadgetConverterTest \
	GeomConverterTest \
	JsonConverterTest \
	ListConverterTest \
	LocaleConverterTest \
	MapConverterTest \
	ObjectConverterTest \
	PairConverterTest \
	RegexConverterTest \
	TupleConverterTest \
	VersionConverterTest

for(test, CONVERTER_TESTS) {
	SUBDIRS += $$test
	$${test}.depends += TypeConverterTestLib
}

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
