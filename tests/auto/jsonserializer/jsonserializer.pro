TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest \
    TypeConverterTest

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
