TEMPLATE = subdirs

SUBDIRS += \
	TypeConverterTestLib \
	SerializerTest

CONVERTER_TESTS = \
	BytearrayConverterTest \
	CborConverterTest \
	ChronoDurationConverterTest \
	EnumConverterTest \
	GadgetConverterTest \
	GeomConverterTest \
	LegacyGeomConverterTest \
	ListConverterTest \
	LocaleConverterTest \
	MapConverterTest \
	MultiMapConverterTest \
	ObjectConverterTest \
	OptionalConverterTest \
	PairConverterTest \
	SmartPointerConverterTest \
#	TupleConverterTest \
	VariantConverterTest \
	VersionConverterTest

for(test, CONVERTER_TESTS) {
	SUBDIRS += $$test
	$${test}.depends += TypeConverterTestLib
}

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
