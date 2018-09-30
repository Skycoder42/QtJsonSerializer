TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest \
	TypeConverterTestLib \
	BytearrayConverterTest \
	GadgetConverterTest \
	GeomConverterTest

never: SUBDIRS += TypeConverterTest

BytearrayConverterTest.depends += TypeConverterTestLib
GadgetConverterTest.depends += TypeConverterTestLib
GeomConverterTest.depends += TypeConverterTestLib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
