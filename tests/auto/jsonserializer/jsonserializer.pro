TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest \
	TypeConverterTestLib \
	BytearrayConverterTest \
	GadgetConverterTest \
	GeomConverterTest \
	JsonConverterTest \
	ListConverterTest \
	LocaleConverterTest

never: SUBDIRS += TypeConverterTest

BytearrayConverterTest.depends += TypeConverterTestLib
GadgetConverterTest.depends += TypeConverterTestLib
GeomConverterTest.depends += TypeConverterTestLib
JsonConverterTest.depends += TypeConverterTestLib
ListConverterTest.depends += TypeConverterTestLib
LocaleConverterTest.depends += TypeConverterTestLib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
