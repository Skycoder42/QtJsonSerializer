TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest \
	TypeConverterTestLib \
	BytearrayConverterTest \
	GadgetConverterTest \
	GeomConverterTest \
	JsonConverterTest

never: SUBDIRS += TypeConverterTest

BytearrayConverterTest.depends += TypeConverterTestLib
GadgetConverterTest.depends += TypeConverterTestLib
GeomConverterTest.depends += TypeConverterTestLib
JsonConverterTest.depends += TypeConverterTestLib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
