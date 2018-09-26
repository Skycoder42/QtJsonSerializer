load(qt_parts)

SUBDIRS += doc

doxygen.target = doxygen
doxygen.CONFIG += recursive
doxygen.recurse_target = doxygen
QMAKE_EXTRA_TARGETS += doxygen lrelease

runtests.target = run-tests
runtests.CONFIG = recursive
runtests.recurse_target = run-tests
runtests.recurse += sub_tests sub_src
QMAKE_EXTRA_TARGETS += runtests

DISTFILES += .qmake.conf \
	sync.profile
