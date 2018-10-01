TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest \
	TypeConverterTestLib

never: SUBDIRS += TypeConverterTest

CONVERTER_TESTS = \
	BytearrayConverterTest \
	GadgetConverterTest \
	GeomConverterTest \
	JsonConverterTest \
	ListConverterTest \
	LocaleConverterTest \
	MapConverterTest \
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
