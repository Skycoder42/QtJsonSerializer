TEMPLATE = subdirs

SUBDIRS += \
	TypeConverterTestLib

CONVERTER_TESTS = \
	BytearrayConverterTest \
	ChronoDurationConverterTest \
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
	VersionConverterTest \
	MultiMapConverterTest

for(test, CONVERTER_TESTS) {
	SUBDIRS += $$test
	$${test}.depends += TypeConverterTestLib
}

SUBDIRS += \
	SerializerTest

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
