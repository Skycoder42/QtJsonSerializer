TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
