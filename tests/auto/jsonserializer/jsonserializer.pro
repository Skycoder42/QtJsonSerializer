TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest \
	TypeConverterTestLib \
	BytearrayConverterTest

never: SUBDIRS += TypeConverterTest

BytearrayConverterTest.depends += TypeConverterTestLib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
